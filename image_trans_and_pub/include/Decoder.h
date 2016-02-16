#ifndef DECODER_H
#define	DECODER_H

#define INBUF_SIZE 1024
#define FRAMEBUFFERSIZE (1920*1080*4)

extern "C"{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#include <libswscale/swscale.h>
#endif
}

extern "C"{
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
}

class Decoder
{
public:
	void				Init();
	void				Release();
	bool				AddStream( char *pStream, unsigned int nLen, int& nGotFrame );
	const char*			GetFrameBuffer();
	int					GetFrameSize();
public:
	//static 
	static bool			InitDecoderLib();
private:
	char m_pYUVFramebuffer[FRAMEBUFFERSIZE];
	AVCodecParserContext*		m_pCodecPaser;
	AVCodecContext*				m_pContext;	
	AVCodec*					m_pCodec;		//解码器 ;
	AVFrame*					m_pFrame;  //解码后的Yuv
	AVFrame*					m_pFrameYUV;
	int							m_iWidth;
	int							m_iHeigth;
	int							m_iYUVSize;
	SwsContext					*m_convert_ctx;
	uint8_t *m_rgbBuffer;
};

int YUV2RGB(char *pDst);

#endif
