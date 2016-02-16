#ifndef LOG_H
#define LOG_H

//#include "AutoLock.h"
#include <stdio.h>
#define CACHESIZE (1024*4)

class Log
{
public:
	Log();
	~Log();
	void Init();
	void Flush();
	void Cache( const char *input, ... );
	void Debug( const char *input, ... );
	static Log* GetSington();
private:
	FILE *m_pFile;
//	static AutoInitLock     s_autoInitLock;
	static Log				*s_pSington;
	char					m_log[CACHESIZE];
	int						m_nCacheSize;
};

#endif