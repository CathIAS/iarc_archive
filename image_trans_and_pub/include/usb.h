#ifndef USB_H
#define	USB_H
#include "libusb-1.0/libusb.h"
#include <stdio.h>

#define CAMERANUM 4

extern unsigned int g_uCameraNum;

bool ShowUsbList();

bool InitUsb();

void ReleaseUsb();

int SyncRecvBuffer( unsigned int uIndex, uint8_t* buffer, int bufferSize, int *readedSize );

#endif
