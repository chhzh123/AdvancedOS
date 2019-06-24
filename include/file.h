// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** FILE.h ******/

#ifndef FILE_H
#define FILE_H

typedef struct _FILE {

	char        name[32];
	char        mode[4];
	char        buffer[1024];
	char*       pos;
	uint32_t    fileLength;
	uint32_t    startCluster;
	uint32_t    currentCluster;

}FILE, *PFILE;

FILE* fopen(const char *pname, const char *mode)
{
	uint32_t ret;
	asm volatile (
		"int 0x82\n\t"
		:"=a"(ret)
		:"a"(0),"b"((uint32_t)pname),"S"((uint32_t)mode)
		);
	return (FILE*) ret;
}

int fclose(FILE* fp)
{
	int ret;
	asm volatile (
		"int 0x82\n\t"
		:"=a"(ret)
		:"a"(1),"b"((uint32_t)fp)
		);
	return ret;
}

int fread(void *buf, int size, int count, FILE *fp)
{
	int ret;
	asm volatile (
		"int 0x82\n\t"
		:"=a"(ret)
		:"a"(2),"D"((uint32_t)fp),"b"((uint32_t)buf),"S"((uint32_t)size),"d"(count)
		);
	return ret;
}

int fwrite(void *buf, int size, int count, FILE *fp)
{
	int ret;
	asm volatile (
		"int 0x82\n\t"
		:"=a"(ret)
		:"a"(3),"D"((uint32_t)fp),"b"((uint32_t)buf),"S"((uint32_t)size),"d"(count)
		);
	return ret;
}

char *fgets(char *str, int count, FILE *stream)
{
	int ret;
	asm volatile (
		"int 0x82\n\t"
		:"=a"(ret)
		:"a"(4),"b"((uint32_t)str),"S"(count),"d"((uint32_t)stream)
		);
	return (char*) ret;
}

int fputs(const char *str, FILE *stream)
{
	int ret;
	asm volatile (
		"int 0x82\n\t"
		:"=a"(ret)
		:"a"(5),"b"((uint32_t)str),"S"((uint32_t)stream)
		);
	return ret;
}

#endif // FILE_H