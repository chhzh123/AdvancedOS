// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** SYSFILE.h ******/

#ifndef SYSFILE_H
#define SYSFILE_H

#include "stdio.h"
#include "fat12.h"
#include <stdarg.h>

#define MAX_FILE_NUM 10

// reference to FreeBSD implementation
// https://en.cppreference.com/w/c/io

typedef struct _FILE {

	char        name[32];
	char        mode[4];
	char        buffer[1024];
	char*       pos;
	uint32_t    fileLength;
	uint32_t    startCluster;
	uint32_t    currentCluster;

}FILE, *PFILE;

static FILE file_list[MAX_FILE_NUM];

void file_init()
{
	for(int i = 0; i < MAX_FILE_NUM; ++i){
		memset(file_list[i].name,0,sizeof(file_list[i].name));
		memset(file_list[i].buffer,0,sizeof(file_list[i].buffer));
		strcpy(file_list[i].mode,"u");
		file_list[i].pos = file_list[i].buffer;
		file_list[i].fileLength = 0;
		file_list[i].currentCluster = file_list[i].startCluster = 0;
	}
}

FILE* do_fopen(const char *pname, const char *mode)
{
#ifdef DEBUG
	printf("Name:%s %s\n", pname, mode);
#endif
	FILE* file = NULL;
	for (int i = 0; i < MAX_FILE_NUM; ++i)
		if (strcmp(file_list[i].mode,"u") == 0){
			file = &file_list[i];
			break;
		}
	if (file == NULL)
		return NULL;
	strcpy(file->name,pname);
	FileEntry_t* fe = fat12_find_entry((char*)pname);
	if (fe == NULL && strcmp(mode,"w") == 0){
		strcpy(file->mode,mode);
		return file;
	} else if (fe == NULL && strcmp(mode,"r") == 0)
		return NULL;
	file->fileLength = fe->fileLength;
	file->currentCluster = file->startCluster = fe->startCluster;
	strcpy(file->mode,mode);
	fat12_read_clusters(file->buffer,file->startCluster);
	return file;
}

int do_fclose(FILE* fp)
{
	if (strcmp(fp->mode,"w") == 0)
		fat12_create_file((uintptr_t)fp->buffer,fp->pos-fp->buffer,fp->name);
	strcpy(fp->mode,"u");
	enable();
	return 0;
}

/*
 * buf	-	pointer to the array where the read objects are stored
 * size	-	size of each object in bytes
 * count	-	the number of the objects to be read
 * fp	-	the stream to read
 */
int do_fread(void *buf, int size, int count, FILE *fp)
{
	int numBytes = size * count;
	memcpy((char*)buf,fp->buffer,numBytes);
	return count;
}

int do_fwrite(void *buf, int size, int count, FILE *fp)
{
	if (strcmp(fp->mode,"w") != 0)
		return 0;
	int numBytes = size * count;
	memcpy(fp->pos,(const char*)buf,numBytes);
	fp->pos = fp->pos + numBytes;
	return count;
}

char *do_fgets(char *str, int count, FILE *fp)
{
	memcpy((char*)str,fp->buffer,count);
	return str;
}

int do_fputs(const char *str, FILE *fp)
{
	if (strcmp(fp->mode,"w") != 0)
		return 0;
	memcpy(fp->pos,str,strlen(str));
	fp->pos = fp->pos + strlen(str);
	return 1;
}

// int do_fprintf(FILE* fp, const char* format, ...)
// {
// 	int ret;
// 	va_list ap;
// 	va_start(ap, fmt);
// 	ret = vfprintf(fp, fmt, ap);
// 	va_end(ap);
// 	return (ret);
// }

#endif // SYSFILE_H