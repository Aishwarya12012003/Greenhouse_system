/* 
 * File:   StdLibrary.h
 * Author: Koustubh Gabale
 *
 * Created on 4 March, 2015, 7:57 PM
 */

#ifndef STDLIBRARY_H
#define	STDLIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

char *itoa(char * buf, int val, int base);
int atoi(const char * s);
int atoi(const char * s);
char isDigit(char c);
char *utoa(char * buf, unsigned val, int base);
unsigned char strLength(const char * s);

#ifdef	__cplusplus
}
#endif

#endif	/* STDLIBRARY_H */

