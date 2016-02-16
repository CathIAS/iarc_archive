#include "libusb-1.0/libusb.h"
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include "usb.h"
#include "config.h"
#include "common.h"
using namespace std;

#define __DEBUG__ 1

libusb_device_handle* g_hdev[CAMERANUM];
libusb_device* g_dev[CAMERANUM];
unsigned int g_uCameraNum = 0;
#define VENDORID 0x0547

static void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0;

	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
		{
			fprintf(stderr, "failed to get device descriptor");
			return;
		}

		printf("%04x:%04x (bus %d, device %d)\n",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));
	}
}

bool ShowUsbList()
{
	int result = libusb_init(NULL);
	if(result  != LIBUSB_SUCCESS)
	{
		printf("Failed to initialise libusb. libusb error: %d\n", result);
		return false;
	}

	struct libusb_device **devs;
	int ret = libusb_get_device_list(NULL, &devs) ;
	if (ret < 0)
	{
		printf("libusb_get_device_list FAILED !!! \n");
		return false;
	}
	else
	{
		printf("llibusb_get_device_list ret = %d !!! \n",ret);
		print_devs(devs);
		libusb_free_device_list(devs, 1);
	}
	return true;
}

bool InitUsb()
{
	if ( 1 != GetConfig("usbon") )
	{
		//读取文件模拟读取usb，则摄像头个数从配置文件中读取，否则，摄像头个数根据实际检测到的usb个数来决定
		if ( sscanf(g_config["cameranum"].c_str(),"%d",&g_uCameraNum) != 1 )
		{
			printf("camera num error!");
			return false;
		}
		if ( g_uCameraNum > CAMERANUM || g_uCameraNum < 1 )
		{
			printf("camera num wrong!\n");
			return false;
		}
	}
	if ( 1 == GetConfig("usbon") )
	{		
		int result = libusb_init(NULL);
		if(result  != LIBUSB_SUCCESS)
		{
			printf("Failed to initialise libusb. libusb error: %d\n", result);
			return false;
		}

		struct libusb_device **devs;
		int ret = libusb_get_device_list(NULL, &devs) ;
		if (ret < 0)
		{
			printf("libusb_get_device_list FAILED !!! \n");
			return false;
		}
		else
		{
			printf("llibusb_get_device_list ret = %d !!! \n",ret);
			print_devs(devs);
		}

		unsigned int productid[CAMERANUM] = {0};

		/*
		int busid[CAMERANUM] = {-1,-1,-1,-1};
		int deviceid[CAMERANUM] = {-1,-1,-1,-1};
		if ( INVALIDCONFIG != GetConfig("busid_front") )
		{
			busid[0] = GetConfig("busid_front");
			deviceid[0] = GetConfig("deviceid_front");
			busid[1] = GetConfig("busid_back");
			deviceid[1] = GetConfig("deviceid_back");
			busid[2] = GetConfig("busid_left");
			deviceid[2] = GetConfig("deviceid_left");
			busid[3] = GetConfig("busid_right");
			deviceid[3] = GetConfig("deviceid_right");
			for ( int nCameraIndex = 0; nCameraIndex < CAMERANUM; ++nCameraIndex )
			{
				if ( -1 == busid[nCameraIndex] || -1 == deviceid[nCameraIndex] )
				{
					printf("请配置前后左右思路摄像头数据对应的busid和deviceid\n");
					return false;
				}
			}
		}
		*/

		g_uCameraNum = 0;
		for ( int i = 0; NULL != devs[i]; ++i )
		{
			libusb_device *dev = devs[i];
			struct libusb_device_descriptor desc;
			int r = libusb_get_device_descriptor(dev, &desc);
			if (r < 0)
			{
				fprintf(stderr, "failed to get device descriptor");
				continue;
			}

			if ( VENDORID == desc.idVendor )
			{
				int nCameraIndex = g_uCameraNum;//当前usb的busid和deviceid对应的前后左右摄像头的下标
				/*
				int nBusId = libusb_get_bus_number(dev);
				int nDeviceId = libusb_get_device_address(dev);
				for ( int nUsbIndex = 0; nUsbIndex < CAMERANUM; ++nUsbIndex )
				{
					if ( busid[nUsbIndex] == nBusId && deviceid[nUsbIndex] == nDeviceId )
					{
						nCameraIndex = nUsbIndex;
						break;
					}
				}

				if ( -1 == nCameraIndex )
				{
					printf("busid:%d,devideid:%d不在配置文件中。\n", nBusId, nDeviceId );
					return false;
				}
				*/

				//GetUsbIndex();
				r = libusb_open(dev, &g_hdev[nCameraIndex]);
				if (r < 0)
				{
					printf("open failed!\n");
					g_hdev[nCameraIndex] = NULL;
					continue;
				}

				if(g_hdev[nCameraIndex] == NULL)
				{
					printf("Could not open device VENDOR_ID = 0x0%x PRODUCT_ID = 0x0%x \n",VENDORID, productid[nCameraIndex]);
					return false;
				}
				g_dev[nCameraIndex] = libusb_get_device(g_hdev[nCameraIndex]);

				result = libusb_claim_interface(g_hdev[nCameraIndex], 0);
				if (result != LIBUSB_SUCCESS)
				{
					printf("Claiming libusb_claim_interface failed!\n");
					libusb_detach_kernel_driver(g_hdev[nCameraIndex], 0);
					result = libusb_claim_interface(g_hdev[nCameraIndex], 0);
				}
				if (result != LIBUSB_SUCCESS)
				{
					printf("Claiming libusb_claim_interface failed!\n");
					libusb_detach_kernel_driver(g_hdev[nCameraIndex], 0);
				}
				++g_uCameraNum;
			}
		}

		libusb_free_device_list(devs, 1);//统计完了usb之后才释放
	}

	return true;
}

void ReleaseUsb()
{
    libusb_exit(NULL);
}

int strlen(const char* p)
{
	int i = 0;
	for ( ; *p != '\0'; ++i, ++p )
	{
	}
	return i;
}

int SyncRecvBuffer(unsigned int uIndex,uint8_t* buffer, int bufferSize, int *readedSize)
{
	libusb_device_handle* hdev = g_hdev[uIndex];
	if(!hdev)
	{
		printf("hdev NULL!  %s %d\n",__FILE__,__LINE__);
		return -1;
	}

	int actural_length = 0;
	int nRet = libusb_bulk_transfer(hdev, 0x86, buffer, bufferSize, &actural_length, 0);// LIBUSB_ENDPOINT_IN | LIBUSB_ENDPOINT_OUT

	//printf("bufferSize %d actural_length %d\n",bufferSize,actural_length);
	if(0 == nRet && (actural_length <= bufferSize))
	{
		*readedSize = actural_length;
		return 1;
	}
	else
	{
		printf("readusb error!  %s %d\n",__FILE__,__LINE__);
		return -1;
	}

	return 1;
}

bool writen(char *buffer, int nLen, FILE *fp)
{
	if ( NULL == fp || NULL == buffer || nLen <=0 )
	{
		return false;
	}

	for ( int nWrite = 0; nWrite < nLen; )
	{
		int n = fwrite( buffer + nWrite, 1, nLen - nWrite, fp );
		if ( n <=0 )
		{
			printf("error!write failed\n");
			return false;
		}
		nWrite += n;
	}

	return true;
}

bool readn(char *buffer, int nLen, FILE *fp)
{
	if ( NULL == fp || NULL == buffer || nLen <=0 )
	{
		return false;
	}

	for ( int nRead = 0; nRead < nLen; )
	{
		int n = fread( buffer + nRead, 1, nLen - nRead, fp );
		if ( n <=0 )
		{
			printf("error!read failed\n");
			return false;
		}
		nRead += n;
	}

	return true;
}
