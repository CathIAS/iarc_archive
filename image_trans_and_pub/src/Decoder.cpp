#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "Decoder.h"
#include "log.h"
#include "config.h"
#include <iostream>
using namespace std;

Log *g_pLogDecoder = NULL;
pthread_mutex_t decoderLibLock = PTHREAD_MUTEX_INITIALIZER;

bool Decoder::InitDecoderLib()
{
	static bool isInitFFmpeg =false;
	if (isInitFFmpeg ==false)
	{
		avcodec_init();
		avcodec_register_all();
		isInitFFmpeg =true;
		g_pLogDecoder = Log::GetSington();
	}

	return true;
}

void Decoder::Init()
{
	m_convert_ctx = NULL;
	//支持多线程
	pthread_mutex_lock(&decoderLibLock);
	m_pCodec = avcodec_find_decoder(CODEC_ID_H264);
	if (!m_pCodec)
	{
		fprintf(stderr, "Codec not found\n");
		g_pLogDecoder->Debug("Codec not found");
		exit(1);
	}
	else
	{
		printf("find decoder!\n");
		g_pLogDecoder->Debug("find decoder");
	}

	m_pContext = avcodec_alloc_context3(m_pCodec);
	if (!m_pContext)
	{
		fprintf(stderr, "Could not allocate video codec context\n");
		g_pLogDecoder->Debug("Could not allocate video codec context");
		exit(1);
	}

	if(m_pCodec->capabilities&CODEC_CAP_TRUNCATED)
	{
		m_pContext->flags|= CODEC_FLAG_TRUNCATED;
	}

	if (avcodec_open2(m_pContext, m_pCodec, NULL) < 0)
	{
		fprintf(stderr, "Could not open codec\n");
		g_pLogDecoder->Debug("Could not open codec");
		exit(1);
	}

	m_pFrame = avcodec_alloc_frame();
	if (!m_pFrame)
	{
		fprintf(stderr, "Could not allocate video frame\n");
		g_pLogDecoder->Debug("Could not allocate video frame");
		exit(1);
	}

	m_pFrameYUV = avcodec_alloc_frame();
	if (!m_pFrameYUV)
	{
		fprintf(stderr, "Could not allocate video m_pFrameYUV\n");
		g_pLogDecoder->Debug("Could not allocate video m_pFrameYUV");
		exit(1);
	}

	m_pCodecPaser = av_parser_init(CODEC_ID_H264);
	if (!m_pCodecPaser)
	{
		fprintf(stderr, "m_pCodecPaser NULL\n");
		g_pLogDecoder->Debug("m_pCodecPaser NULL");
		exit(1);
	}

	m_rgbBuffer = NULL;

	pthread_mutex_unlock(&decoderLibLock);
}

const char*		Decoder::GetFrameBuffer()
{
	if ( GetConfig("rgbformat") )
	{
		return (const char*)m_rgbBuffer;
	}
	else
	{
		return m_pYUVFramebuffer;
	}
}

int			Decoder::GetFrameSize()
{
	return m_iYUVSize;
}

void Decoder::Release()
{
	avcodec_close(m_pContext);
	av_free(m_pContext);
	av_free(m_pFrame);
}

bool	Decoder::AddStream( char *pStream, unsigned int nLen, int& nGotFrame )
{
	int paserLength_In = nLen;
	int paserLen;
	int decode_data_length;
	uint8_t *pFrameBuff = (uint8_t*)pStream;

	while (paserLength_In >0) 
	{
		AVPacket packet;
		av_init_packet(&packet);

		paserLen = av_parser_parse2(m_pCodecPaser, m_pContext, &packet.data, &packet.size, pFrameBuff, paserLength_In,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		paserLength_In -= paserLen;
		pFrameBuff += paserLen;

		if (packet.size > 0)
		{
			decode_data_length = avcodec_decode_video2(m_pContext, m_pFrame, &nGotFrame, &packet);
			if ( 0 == nGotFrame )
			{
				continue;
			}
			else
			{
				if (m_iWidth!=m_pContext->width||m_iHeigth!=m_pContext->height)
				{
					m_iWidth = m_pContext->width; 
					m_iHeigth=m_pContext->height;
				}
				static int s_nDebugCount = 0;
				++s_nDebugCount;
				if ( 100 == s_nDebugCount )
				{
					printf( "width %d m_iHeigth %d %d\n", m_iWidth, m_iHeigth, __LINE__ );
					g_pLogDecoder->Debug( "width %d m_iHeigth %d %d", m_iWidth, m_iHeigth, __LINE__ );
				}

				if ( GetConfig("rgbformat"))
				{
					//转换
					if ( NULL == m_rgbBuffer )
					{
						m_rgbBuffer = new uint8_t[avpicture_get_size(PIX_FMT_RGB24, m_pContext->width, m_pContext->height)];
						if ( NULL == m_rgbBuffer )
						{
							printf("error m_rgbBuffer null!\n");
							g_pLogDecoder->Debug("error m_rgbBuffer null!");
						}
						avpicture_fill((AVPicture *)m_pFrameYUV, m_rgbBuffer, PIX_FMT_RGB24, m_pContext->width, m_pContext->height);
					}
					if ( NULL == m_convert_ctx )
					{
						m_convert_ctx = sws_getContext(m_pContext->width, m_pContext->height, m_pContext->pix_fmt, m_pContext->width, m_pContext->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL); 
					}
					sws_scale(m_convert_ctx, (const uint8_t* const*)m_pFrame->data, m_pFrame->linesize, 0, m_pContext->height, m_pFrameYUV->data, m_pFrameYUV->linesize);

					int nPixelCount = m_pContext->width*m_pContext->height;
					char *p1 = (char*)m_rgbBuffer;
					char *p2 = (char*)m_rgbBuffer + 1;
					char *p3 = (char*)m_rgbBuffer + 2;
					for ( int nPixelIndex = 0; nPixelIndex < nPixelCount; ++nPixelIndex )
					{
						char c = *p1;
						*p1 = *p3;
						*p3 = c;

						p1 += 3;
						p2 += 3;
						p3 += 3;
					}
					m_iYUVSize = m_pContext->width*m_pContext->height*3;
				}
				else
				{
					m_iYUVSize = 0;
					for (int i = 0;i<3;i++)
					{
						int nShift = (i==0)?0:1;
						uint8_t* pYUVData = (uint8_t*)m_pFrame->data[i];

						for (int j = 0;j<(m_pContext->height>>nShift);j++)
						{
							memcpy(&m_pYUVFramebuffer[m_iYUVSize],pYUVData,(m_pContext->width>>nShift));
							pYUVData+=m_pFrame->linesize[i];
							m_iYUVSize+=(m_pContext->width>>nShift);
						}
					}
				}

				if ( 100 == s_nDebugCount )
				{
					s_nDebugCount = 0;
				}
			}
		} 
		else 
		{
			continue;
		}
		av_free_packet(&packet);
	}

	return true;//暂时没有什么错误分支
}
