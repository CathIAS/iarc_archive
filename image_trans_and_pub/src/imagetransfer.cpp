#include "Decoder.h"
#include "imagetransfer.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "usb.h"
#include "common.h"
#include "config.h"
#include "log.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

#define VIDEOBUFFERSIZE (100*1024*1024)

extern unsigned int g_uCameraNum;

bool s_bTransfer = false;
pthread_mutex_t imageLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t videoLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
//char g_pYUVFramebuffer[CAMERANUM][FRAMEBUFFERSIZE];
char g_pRGBFramebuffer[CAMERANUM][RGBBUFFERSIZE];
int g_nRealSize[CAMERANUM] = {0,0,0,0};
char g_pViedoBuffer[CAMERANUM][VIDEOBUFFERSIZE];
int g_nHeader[CAMERANUM] = {0,0,0,0};
int g_nTail[CAMERANUM] = {0,0,0,0};
int g_nCount[CAMERANUM] = {0,0,0,0};
Log *g_pLog = NULL;
char g_szPath[256] = {0};

bool GetImage( ECAMERAINDEX eCameraIndex, char *pDstBuffer )
{
	if ( eCameraIndex <0 || (unsigned int)eCameraIndex >= g_uCameraNum || (NULL == pDstBuffer) )
	{
		printf("wrong camera index!\n");
		return false;
	}

	pthread_mutex_lock(&videoLock);
	memcpy( pDstBuffer, g_pRGBFramebuffer[eCameraIndex], RGBBUFFERSIZE );
	pthread_mutex_unlock(&videoLock);
	return true;
}

void* DecodeFun( void *param )
{
	unsigned int *pIndex = (unsigned int*)param;
	unsigned int uIndex = *pIndex;

	FILE *pRGBOutputFile = NULL;
	if ( 1 == GetConfig("saveoutput") )
	{
		char szRGBFileName[32] = {0};
		if ( GetConfig("rgbformat") )
		{
			snprintf( szRGBFileName, sizeof(szRGBFileName), "%s/thread%d.rgb",g_szPath, (uIndex+1));
		}
		else
		{
			snprintf( szRGBFileName, sizeof(szRGBFileName), "%s/thread%d.yuv", g_szPath, (uIndex+1) );
		}
		pRGBOutputFile = fopen( szRGBFileName, "wb" );
		printf("open output file! %s %d threadid:%d\n",__FILE__, __LINE__, (int)(uIndex+1));
		if ( NULL == pRGBOutputFile )
		{
			printf("error! open output file failed!\n threadid:%d",(uIndex+1));
			g_pLog->Debug("error! open output file failed! threadid:%d",(uIndex+1));
		}
	}

	Decoder decoder;
	decoder.Init();

	timeval t1;
	gettimeofday( &t1, NULL );

	FILE *pVideoFile = NULL;
	if ( 1 == GetConfig("savevideo") )
	{
		char szVideoFileName[32] = {0};
		snprintf(szVideoFileName, sizeof(szVideoFileName), "%s/thread%d.h264",g_szPath,(uIndex+1));
		pVideoFile = fopen( szVideoFileName, "wb" );
		if ( NULL == pVideoFile )
		{
			g_pLog->Debug("open video file failed threadid:%d",(uIndex+1));
		}
		else
		{
			g_pLog->Debug("open video file ok threadid:%d",(uIndex+1));
		}
	}

	int nDebugCount = 0;
	int nMaxDebugCount = 1;
	for ( ; s_bTransfer; )
	{
		++nDebugCount;

		char buffer[INBUF_SIZE] = {0};
		int nReadSize = INBUF_SIZE;
		if ( nMaxDebugCount == nDebugCount )
		{
			g_pLog->Debug( "%s %d threadid:%d",__FILE__, __LINE__, (uIndex+1) );
		}
		for ( ; 0 == g_nCount[uIndex]; )
		{
		//	printf( "%s %d\n", __FILE__, __LINE__ );
			usleep(1000);
		}
		if ( nMaxDebugCount == nDebugCount )
		{
			g_pLog->Debug( "%s %d threadid:%d",__FILE__, __LINE__, (uIndex+1) );
		}
		pthread_mutex_lock(&imageLock);
		if ( g_nCount[uIndex] < nReadSize )
		{
			nReadSize = g_nCount[uIndex];
		}
		if ( g_nHeader[uIndex] + nReadSize <= VIDEOBUFFERSIZE )
		{
			memcpy( buffer, g_pViedoBuffer[uIndex] + g_nHeader[uIndex], nReadSize );
			g_nHeader[uIndex] += nReadSize;
		}
		else
		{
			int nLen1 = VIDEOBUFFERSIZE - g_nHeader[uIndex];
			int nLen2 = nReadSize - nLen1;
			memcpy( buffer, g_pViedoBuffer[uIndex] + g_nHeader[uIndex], nLen1 );
			memcpy( buffer + nLen1, g_pViedoBuffer[uIndex], nLen2 );
			g_nHeader[uIndex] = nLen2;
		}
		g_nCount[uIndex] -= nReadSize;
		if ( VIDEOBUFFERSIZE == g_nHeader[uIndex] )
		{
			g_nHeader[uIndex] = 0;
		}
		pthread_mutex_unlock(&imageLock);
		if ( nMaxDebugCount == nDebugCount )
		{
			g_pLog->Debug( "%s %d\n threadid:%d",__FILE__, __LINE__, (uIndex+1) );
		}
		if ( 1 == GetConfig("savevideo") )
		{
			bool bRet = writen(buffer,nReadSize,pVideoFile);
//			printf("ret:%d  readsize:%d\n",(int)bRet,nReadSize);
			if ( !bRet )
			{
				g_pLog->Debug("write failed threadid:%d", (uIndex+1));
			}
		}
		int nGotFrame = 0;
		if ( 1 == GetConfig("statistic") )
		{
		//	continue;//只是统计，不需要解码
		}
		bool bSuccess = decoder.AddStream( buffer, (unsigned int)nReadSize, nGotFrame );
		if ( nMaxDebugCount == nDebugCount )
		{
		    g_pLog->Debug( "%s %d\n threadid:%d",__FILE__, __LINE__, (uIndex+1)  );
		}
		if ( bSuccess )
		{
			if ( nGotFrame )
			{
				static int nCount = 0;
				++nCount;
				int nTime = getDeltaTime(t1);
				if ( nMaxDebugCount == nDebugCount )
				{
					g_pLog->Debug( "%s %d time:%d count:%d ***************************************************** threadid:%d", 
						__FILE__, __LINE__,nTime,nCount, (uIndex+1) );
				}
				int nFrameSize = decoder.GetFrameSize();
				const char *pFrameBuffer = decoder.GetFrameBuffer();
                pthread_mutex_lock(&videoLock);
                memcpy( g_pRGBFramebuffer[uIndex], pFrameBuffer, nFrameSize );
                pthread_mutex_unlock(&videoLock);
				if ( 1 == GetConfig("saveoutput") )
				{
					bool bRet = writen( (char*)pFrameBuffer, nFrameSize, pRGBOutputFile );
					if ( !bRet )
					{
						printf("save output error! %s %d threadid:%d\n",__FILE__,__LINE__,(uIndex+1));
					}
			//		printf("save rgb file ok! %s %d threadid:%d\n",__FILE__, __LINE__, (int)(uIndex+1));
				}
				if ( nMaxDebugCount == nDebugCount )
				{
					g_pLog->Debug( "%s nFrameSize:%d  %d threadid:%d", __FILE__,nFrameSize, __LINE__, (uIndex+1) );
					//printf( "%s nFrameSize:%d  %d threadid:%d\n", __FILE__,nFrameSize, __LINE__, (uIndex+1) );
				}
			}
		}
		else
		{
			g_pLog->Debug( "break! threadid:%d", (uIndex+1) );
			break;
		}

		if ( nMaxDebugCount == nDebugCount )
		{
			nDebugCount = 0;
		}
	}

	fclose(pRGBOutputFile);
	pRGBOutputFile = NULL;

	pthread_cond_signal(&qready);//给GetImage一个退出的机会
	printf( "%s %d\n threadid:%d",__FILE__, __LINE__, (uIndex+1)  );
	g_pLog->Debug( "%s %d threadid:%d",__FILE__, __LINE__, (uIndex+1) );
	decoder.Release();

	fclose(pVideoFile);
	pVideoFile = NULL;

	return NULL;	
}

void* TransferFun( void *param )
{
	bool bUsbOn = true;
	if ( 1 != GetConfig("usbon") )
	{
		bUsbOn = false;
	}

	unsigned int *pIndex = (unsigned int*)param;
	unsigned int uIndex = *pIndex;

	FILE *pVideoFile = NULL;
	if ( !bUsbOn )
	{
		char szKey[32] = {0};
		snprintf( szKey, sizeof(szKey), "h264_%d", (uIndex+1) );
		string strInputFileName = g_config[szKey];
		char *pFileName = (char*)strInputFileName.c_str();
		pVideoFile = fopen( pFileName, "rb" );
		if ( NULL == pVideoFile )
		{
			printf("error! open file %s failed! key:%s %s %d\n",strInputFileName.c_str(), szKey, __FILE__,__LINE__);
			return 0;
		}
	}

	int nReadTimes = 0;//读取usb多少次
	timeval t1;
	gettimeofday( &t1, NULL );

	FILE *fpStatistic = NULL;
	if ( 1 == GetConfig("statistic") )
	{
		char szStatisticFile[32] = {0};
		snprintf( szStatisticFile, sizeof(szStatisticFile), "%s/statistic%d.txt",g_szPath, (uIndex+1));
		fpStatistic = fopen( szStatisticFile, "wb" );
	}

	for ( ; s_bTransfer; )
	{
	//	for ( int uIndex = 0; uIndex < g_uCameraNum; ++uIndex )
		{
			char buffer[INBUF_SIZE] = {0};
			int nRealReadSize = 0;
			int nToReadSize = INBUF_SIZE;
			g_pLog->Debug( "%s %d threadid:%d",__FILE__, __LINE__, (uIndex+1)  );
			int nRet = 0;
			timeval t2;
			gettimeofday( &t2, NULL );
			if ( bUsbOn )
			{
				nRet = SyncRecvBuffer(uIndex,(uint8_t*)buffer, nToReadSize, &nRealReadSize);
//				printf("ret:%d\n",nRet);
			//	if( nRet <= 0 )
			}
			else
			{
				nRealReadSize = fread( buffer, sizeof(char), nToReadSize, pVideoFile );
			}
			int nTotalTime = getDeltaTime(t1);
			int nThisTime = getDeltaTime(t2);
			++nReadTimes;
			int nAverage = nTotalTime / nReadTimes;
//			printf( "average:%d this step:%d\n",nAverage, nThisTime );

			if( nThisTime > nAverage + nAverage*0.1 )
			{
		//		printf( "threadid:%d tooooooooooooooooooooooooooooooooooo long!\n", (int)uIndex );
			}
			if ( 1 == GetConfig("statistic") && NULL != fpStatistic )
			{
				fprintf( fpStatistic, "threadid:%d totaltime:%d average:%d times:%d thistime:%d\n", 
					(int)uIndex, nTotalTime, nAverage, nReadTimes, nThisTime );
			}
			if ( nRet < 0 || nRealReadSize <= 0 )
			{
				printf("error!SyncRecvBuffer error!%s %dthreadid:%d\n",__FILE__, __LINE__, (uIndex+1) );
				printf("header:%d tail:%d threadid:%d\n",g_nHeader[uIndex],g_nTail[uIndex], (uIndex+1));
				g_pLog->Debug("error!SyncRecvBuffer error!%s %d threadid:%d",__FILE__, __LINE__, (uIndex+1) );
				g_pLog->Debug("header:%d tail:%d threadid:%d",g_nHeader[uIndex],g_nTail[uIndex], (uIndex+1));
				g_nRealSize[uIndex] = -1;//表示结束了
				//			s_bTransfer = false;
				break;
			}
			for ( ; g_nCount[uIndex] > VIDEOBUFFERSIZE - nRealReadSize; )
			{
				printf("wait *****************************************************************threadid%d\n", (uIndex+1));
				g_pLog->Debug("wait **********************************************************threadid%d", (uIndex+1));
				usleep(1000);
			}
			pthread_mutex_lock(&imageLock);
			int nTail = g_nTail[uIndex];
			if ( VIDEOBUFFERSIZE - nTail >= nRealReadSize )
			{
				memcpy( g_pViedoBuffer[uIndex] + nTail, buffer, nRealReadSize );
				g_nTail[uIndex] += nRealReadSize;
			}
			else
			{
				int nLen1 = VIDEOBUFFERSIZE - g_nTail[uIndex];
				int nLen2 = nRealReadSize - nLen1;
				memcpy( g_pViedoBuffer[uIndex] + nTail, buffer, nLen1 );
				memcpy( g_pViedoBuffer[uIndex], buffer + nLen1, nLen2 );
				g_nTail[uIndex] = nLen2;
			}
			g_nCount[uIndex] += nRealReadSize;
			if ( VIDEOBUFFERSIZE == g_nTail[uIndex] )
			{
				g_nTail[uIndex] = 0;
			}
			pthread_mutex_unlock(&imageLock);
			pthread_cond_signal(&qready);//给GetImage一个退出的机会
		}
	}

	fclose(pVideoFile);
	pVideoFile = NULL;

	return NULL;
}

//输出的视频文件存储在文件夹中，文件夹的名字为一个数字，这个数字每次递增1
int GetFilePathIndex()
{
	FILE *fp = fopen( "./index.txt", "a+" );
	int nIndex = 0;
	unsigned int nRet = fread( (char*)&nIndex, sizeof(int), 1, fp );
	if ( nRet != 1 )
	{
		nIndex = 1;
		fseek(fp, SEEK_SET, 0 );
		nRet = fwrite( (char*)&nIndex, sizeof(int), 1, fp );
	}
	else
	{
		int m_fd = open("./index.txt", O_RDWR);
		unsigned char *m_ptr = (unsigned char*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
		nIndex = *m_ptr;
		++nIndex;
		*m_ptr = nIndex;
		munmap(m_ptr, sizeof(int));
	}
	fclose(fp);
	fp = NULL;

	return nIndex;
}

bool InitImageTransfer()
{
	g_pLog = Log::GetSington();
	if ( NULL == g_pLog )
	{
		printf("Log NULL!");
		return false;
	}

	ReadConfig( "/home/iarc/catkin_ws/devel/lib/image_trans_and_pub/config.txt" );

	int nIndex = GetFilePathIndex();
	snprintf( g_szPath, sizeof(g_szPath), "./output_%d", nIndex );
	int status0 = mkdir( g_szPath, 0x777 );
	if ( -1 == status0 )
	{
		printf( "mkdir %s failed!!!!\n", "/data/disparity/" );
		return false;
	}

	bool bSuccess = Decoder::InitDecoderLib();
	if ( !bSuccess )
	{
		g_pLog->Debug( "InitDecoderLib failed" );
		return false;
	}
	g_pLog->Debug( "InitDecoderLib OK" );
	bSuccess = InitUsb();
	if ( !bSuccess )
	{
		g_pLog->Debug( "InitUsb failed" );
		return false;
	}
	g_pLog->Debug( "InitUsb OK" );
	s_bTransfer = true;
	
	unsigned int uThreadId[CAMERANUM] = {0};
	for ( unsigned int i = 0; i < g_uCameraNum; ++i )
	{
		pthread_t id;
		uThreadId[i] = i;
		pthread_create(&id,NULL,DecodeFun,(void*)&uThreadId[i]);
		pthread_create(&id,NULL,TransferFun,(void*)&uThreadId[i]);
	}
	usleep(100000);
	g_pLog->Debug( "TransferFun OK" );
	g_pLog->Debug( "DecodeFun OK" );
	return true;
}

void StopImageTransfer()
{
	s_bTransfer = false;
	usleep(10000);
	ReleaseUsb();
}
