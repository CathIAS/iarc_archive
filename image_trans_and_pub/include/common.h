#ifndef COMMON_H
#define	COMMON_H

bool readn(char *buffer, int nLen, FILE *fp );

bool writen(char *buffer, int nLen, FILE *fp );

int getDeltaTime( struct timeval &rTime1 );

#endif
