#include "log.h"
//#include "autoLock.h"
#include "config.h"
#include <stdarg.h>

//AutoInitLock    Log::s_autoInitLock;
Log*		Log::s_pSington = NULL;

Log::Log(void)
{
	m_pFile = NULL;
	m_nCacheSize = 0;
}

Log::~Log(void)
{
	if ( NULL != m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

Log* Log::GetSington()
{
	if ( NULL == s_pSington )
	{
//		MyAutoLock lock( *s_autoInitLock.GetLock() );
		if ( NULL == s_pSington )
		{
			s_pSington = new Log();
			s_pSington->Init();
		}
	}

	return s_pSington;
}

void Log::Init()
{
	m_pFile =  fopen("log.txt","w+");
}

void Log::Flush()
{
	/*
	if ( m_nCacheSize < CACHESIZE )
	{
		m_log[m_nCacheSize] = '\0';
	}
	else
	{
		m_log[CACHESIZE-1] = '\0';
	}
	*/

	fwrite( m_log, sizeof(char), m_nCacheSize, m_pFile );
	fflush( m_pFile );
	m_nCacheSize = 0;
}

void Log::Cache( const char *input, ... )
{
	if ( m_nCacheSize > (CACHESIZE - 200) )
	{
		Flush();
	}
	va_list   arg;
	char szParam[256];
	va_start(arg,input);  
	vsprintf(szParam,input,arg);
	int nCached = snprintf( m_log+m_nCacheSize, CACHESIZE-m_nCacheSize, szParam );
	if ( nCached > (CACHESIZE-m_nCacheSize) )
	{
		Flush();
		nCached = snprintf( m_log, CACHESIZE, szParam );
	}
	m_nCacheSize += nCached;
	va_end(arg);
}

void Log::Debug( const char *input, ... )
{
	if ( "1" != g_config["log"] )
	{
		return;
	}

	time_t  t = time(0);
	tm localTime = *localtime(&t);
	Cache( "\n%02d-%02d-%02d-%02d-%02d-%02d    ", 1900+localTime.tm_year, localTime.tm_mon,
		    localTime.tm_mday, localTime.tm_hour, localTime.tm_min, localTime.tm_sec );

	va_list   arg;
	char szParam[256];

	va_start(arg,input);  
	vsprintf(szParam,input,arg);
	Cache( szParam );
	va_end(arg);

	Flush();//先不用缓存，保证能及时刷到文件中，便于定位问题
}
