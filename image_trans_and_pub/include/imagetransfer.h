#ifndef IMAGETRANSFER_H
#define	IMAGETRANSFER_H
#include "usb.h"

bool InitImageTransfer();

void StopImageTransfer();

bool GetImage( unsigned int nIndex, char *pImageBuffer, int &uRealSize );

extern bool s_bTransfer;

enum ECAMERAINDEX
{
	front = 0,
	back = 1,
	left = 2,
	right = 3
};

#define WIDTH 736
#define HEIGHT 576
#define RGBBUFFERSIZE (WIDTH*HEIGHT*3)

bool GetImage( ECAMERAINDEX eCameraIndex, char *pDstBuffer );

#endif
