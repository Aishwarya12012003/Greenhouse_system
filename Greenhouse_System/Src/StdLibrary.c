#include "StdLibrary.h"

char *itoa(char * buf, int val, int base)
{
    char *	cp = buf;

    if(val < 0)
    {
        *buf++ = '-';
        val = -val;
    }
    utoa(buf, val, base);
    return cp;
}

int atoi(const char * s)
{
	register int	a;
	register unsigned char	sign;

	while(*s == ' ' || *s == '\t')
		s++;
	a = 0;
	sign = 0;
	if(*s == '-')
        {
		sign++;
		s++;
	}
        else if(*s == '+')
		s++;
	while(isDigit(*s))
		a = a*10 + (*s++ - '0');
	if(sign)
		return -a;
	return a;
}

char isDigit(char c)
{
    return c <= '9' && c >= '0';
}

char *utoa(char * buf, unsigned val, int base)
{
    unsigned v;
    char c;

    v = val;
    do
    {
        v /= base;
        buf++;
    } while(v != 0);
    *buf-- = 0;
    do
    {
        c = val % base;
        val /= base;
        if(c >= 10)
            c += 'A'-'0'-10;
        c += '0';
        *buf-- = c;
    } while(val != 0);
    return ++buf;
}

unsigned char strLength(const char * s)
{
	register const char *	cp;

	cp = s;
	while(*cp) {
		cp++;
	}
	return cp-s;
}

