// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** string.h ******/

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// sets count bytes of dest to val
void *memset(void *dest, char val, size_t count)
{
	unsigned char *temp = (unsigned char *)dest;
	for( ; count != 0; count--, temp[count] = val);
	return dest;
}

void *memcpy(void *dst, const void *src, size_t n) {
	const char *s = src;
	char *d = dst;
	while (n-- > 0) {
		*d ++ = *s ++;
	}
	return dst;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len++]);
	len--;
	return len;
}

int strcmp(const char *s1, const char *s2){
	// = 0, < -1, > 1
	while(*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strcpy(char *strDest, const char *strSrc)
{
	char *temp = strDest;
	while((*strDest++=*strSrc++) != '\0');
	return temp;
}

char* strncpy(char *dest, const char *src, size_t n)
{
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];
	for ( ; i < n; i++)
		dest[i] = '\0';
	return dest;
}

char* strmcpy(char *dest, const char *src, size_t start, size_t end)
{
	// dest = src[start:end]
	size_t i;
	size_t cnt = 0;
	for (i = start; i < end && src[i] != '\0'; i++)
		dest[cnt++] = src[i];
	dest[cnt] = '\0';
	return dest;
}

char* strcat(char* dst, const char* src)
{
	char* ptr = dst + strlen(dst);
	while (*src != '\0')
		*ptr++ = *src++;
	*ptr = '\0';
	return dst;
}

int atoi(const char *str) 
{ 
	int res = 0;
	for (int i = 0; str[i] != '\0'; ++i) 
		res = res*10 + str[i] - '0';
	return res; 
}

void reverse(char* s)
{
	int i, j;
	char c;
	for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

// Find the first occurrence of a character in a string.
char *strchr(const char *s, char c)
{
	for ( ; *s != c; ++s )
		if( *s == '\0' )
			return( NULL );
	return (char*) s;
}

char *strsep(char **str, const char *delims)
{
	char* token;
	if (!*str) {
		/* No more tokens */
		return NULL;
	}
	token = *str;
	while (**str != '\0') {
		if (strchr(delims, **str)) {
			**str = '\0';
			(*str)++;
			return token;
		}
		(*str)++;
	}
	*str = NULL;
	return token;
}

void itoa(unsigned i, char* buf, unsigned base) {
	char tbuf[32];
	char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	int pos = 0;
	int opos = 0;
	int top = 0;

	if (i == 0 || base > 16) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	while (i != 0) {
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}
	top = pos--;
	for (opos = 0; opos < top; pos--, opos++) {
		buf[opos] = tbuf[pos];
	}
	buf[opos] = 0;
}

void itoa_s(int i, char* buf, unsigned base) {
	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}
	itoa(i,buf,base);
}


char* tolower(char* str)
{
	int len = strlen(str);
	for (int i = 0; i < len; ++i)
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] += ('a' - 'A');
	return str;
}

char* toupper(char* str)
{
	int len = strlen(str);
	for (int i = 0; i < len; ++i)
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			str[i] += ('A' - 'a');
	return str;
}

bool isalpha(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

bool isspace(char c)
{
	return c == ' ';
}

bool isnum(char* str)
{
	int i;
	for (i = 0; str[i]; ++i)
		if (!isdigit(str[i]))
			return false;
	return true;
}

bool isin(char* str, char c)
{
	int i;
	for (i = 0; str[i]; ++i)
		if (str[i] == c)
			return true;
	return false;
}

int split(const char* str, char c, int* offset)
{
	// str: the input string
	// c: the split char (where to split)
	// offset: the position of c in str
	int length = strlen(str);
	int i= 0;
	int cnt = 0;
	for (i = 0; i < length; ++i)
		if (str[i] == c)
			offset[cnt++] = i;
	offset[cnt++] = length;
	return cnt;
}

#endif // STRING_H