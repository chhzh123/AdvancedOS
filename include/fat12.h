// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** FAT12.h ******/

#ifndef FAT12_H
#define FAT12_H

#include "ide.h"
#include <stdint.h>

/*
 * FAT12
 *
 * 2 * 18 * 80 = 2880 sectors
 *
 * LogSec | NumSec   | Context
 * 0      | 1 (512)  | BootSec
 * 1      | 9 (4608) | FAT1
 * 10     | 9 (4608) | FAT2
 * 19     | 14 (9728)| root
 * 33     | 14 (9828)| data
 *
 * Reference:
 * MinirighiOS, http://minirighi.sourceforge.net/html/fat_8h-source.html
 * FAT12, https://github.com/imtypist/fat12
 */

#define FAT_SECTOR_SIZE 512
#define FAT_BOOTSECTOR_SIZE 1
#define FAT_PHYS_SIZE 9
#define FAT_ROOT_SIZE 14
#define FAT_NUM_ROOT_ENTRY 224

#define FAT_BOOTSECTOR_START 0
#define FAT_ENTRY_START 1
#define FAT_ROOT_REGION_START 19
#define FAT_DATA_REGION_START 33

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_SUBDIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_DEVICE 0x40
#define FAT_ATTR_UNUSED 0x80

#define EOF_FAT12	   0xFF8

// Boot Sector (BS) & BPB (BIOS Parameter Block)
typedef struct bootsect
{
	uint8_t	      Jump[3]; // 3
	char          BS_OEMName[8]; // 11
	uint16_t      BPB_BytesPerSector; // 13
	uint8_t       BPB_SectorsPerCluster; // 14
	uint16_t      BPB_ReservedSectors; // 16
	uint8_t       BPB_NumFATs; // 17
	uint16_t      BPB_RootDirectoryEntries; // 19
	uint16_t      BPB_LogicalSectors; // 21
	uint8_t       BPB_MediumDescriptorByte; // 22
	uint16_t      BPB_SectorsPerFat; // 24
	uint16_t      BPB_SectorsPerTrack; // 26
	uint16_t      BPB_NumHeads; // 28
	uint32_t      BPB_HiddenSectors; // 32
	uint8_t       code[480]; // the last 2B is signature "AA55h"

} __attribute__ ((packed)) bootsect_t;
static bootsect_t bootsector;

/*
 * File Allocation Table (FAT)
 * which is a linked list, with 12 bits of each entry
 * 0FF0 - 0FFF are bad clusters
 *
 * 9 sectors * 512B/sector = 4608
 * 4608B * 8bits / 12bits = 3072 entries
 *
 */
typedef struct phys_fat
{
	uint8_t entry [FAT_PHYS_SIZE * FAT_SECTOR_SIZE]; // 4608

} __attribute__ ((packed)) phys_fat12_t;
static phys_fat12_t phys_fat;

typedef struct logic_fat
{
	// only the first 3B are used (24bits)
	// the first two clusters (0,1) are useless
	uint32_t entry [3072];

} __attribute__ ((packed)) fat12_t;
static fat12_t fat;

// Root Directory
// 16 directory entries per sector = 512B / 32B = 16
// 14 sectors * 16 = 224 entries
typedef struct FileEntry // 32B
{
	char name[8];
	char extension[3];
	uint8_t attribute;
	uint8_t reserved[10];
	uint16_t time;
	uint16_t date;
	uint16_t startCluster;
	uint32_t fileLength;

} __attribute__ ((packed)) FileEntry_t;

typedef struct fat_date
{
	int year;
	int month;
	int day;

} __attribute__ ((packed)) fat_date_t;

typedef struct fat_time
{
	int hour;
	int minute;
	int second;

} __attribute__ ((packed)) fat_time_t;

typedef struct attrib
{
	bool RW;
	bool Hidden;
	bool System;
	bool Label;
	bool Directory;
	bool Archived;
	uint8_t Reserved;

} __attribute__ ((packed)) fat_attrib_t;

typedef struct root_directory
{
	FileEntry_t entry[FAT_SECTOR_SIZE / sizeof(FileEntry_t) * FAT_ROOT_SIZE]; // 224

} __attribute__ ((packed)) fat_root_t;
static fat_root_t root_dir;

// bool Read_FAT();
// bool load_file(char *stringa, uint8_t *buffer);
// int get_file_size(char *file_name);
// char *pwd();
// void ls();
// bool cd(char *new_path);
// bool cat(char *stringa);
// bool rm(char *filename);

bool fat12_init()
{
	// Boot sector
	read_sectors((uintptr_t)&bootsector,FAT_BOOTSECTOR_START,FAT_BOOTSECTOR_SIZE);

#ifdef DEBUG
	printf("\nInitializing FileSystem...\n");
	printf("\n");
	printf("Jump:%02x %02x %02x\n", bootsector.Jump[0], bootsector.Jump[1], bootsector.Jump[2]);
	printf("OS Name:%s\n", bootsector.BS_OEMName);
	printf("BytesPerSector:%d\n", bootsector.BPB_BytesPerSector);
	printf("SectorsPerCluster:%d\n", bootsector.BPB_SectorsPerCluster);
	printf("ReservedSectors:%d\n", bootsector.BPB_ReservedSectors);
	printf("NumFATs:%d\n", bootsector.BPB_NumFATs);
	printf("RootDirectoryEntries:%d\n", bootsector.BPB_RootDirectoryEntries);
	printf("LogicalSectors:%d\n", bootsector.BPB_LogicalSectors);
	printf("MediumDescriptorByte:%X\n", bootsector.BPB_MediumDescriptorByte);
	printf("SectorsPerFat:%d\n", bootsector.BPB_SectorsPerFat);
	printf("SectorsPerTrack:%d\n", bootsector.BPB_SectorsPerTrack);
	printf("Heads:%d\n", bootsector.BPB_NumHeads);
	printf("HiddenSectors:%d\n", bootsector.BPB_HiddenSectors);
	printf("\n");
#endif

	// FAT sectors
	read_sectors((uintptr_t)&phys_fat,FAT_ENTRY_START,FAT_PHYS_SIZE);

	// Converts the FAT into the logical structures (array of word).
	for (int i = 0, j = 0; i < 4608; i += 3)
	{
		fat.entry[ j++ ] = (phys_fat.entry[i] + (phys_fat.entry[i+1] << 8)) & 0x0FFF;
		fat.entry[ j++ ] = (phys_fat.entry[i+1] + (phys_fat.entry[i+2] << 8)) >> 4;
	}
	printf("FAT1:%d %d %d %d %d %d %d %d %d\n", fat.entry[0], fat.entry[1], fat.entry[2],
		fat.entry[3], fat.entry[4], fat.entry[5], fat.entry[6], fat.entry[7], fat.entry[8]);

	// root directory
	read_sectors((uintptr_t)&root_dir,FAT_ROOT_REGION_START,FAT_ROOT_SIZE);
	for (int i = 0; i < 4; ++i){
		printf("/%s.%s start:%d length:%d\n", root_dir.entry[i].name, root_dir.entry[i].extension,
			root_dir.entry[i].startCluster, root_dir.entry[i].fileLength);
	}

	return true;
}

void int_to_date(fat_date_t* d, uint16_t date)
{
	d->year  = date/512;
	d->month = (date - (d->year * 512)) / 32;
	d->day   = date - (d->year * 512) - (d->month * 32);
}

void int_to_time(fat_time_t* time, uint16_t t)
{
	time->hour   = t / 2048;
	time->minute = (t - (time->hour * 2048)) / 32;
	time->second = (t - (time->hour * 2048) - (time->minute * 32)) * 2;
}

void fat12_construct_file_name(char* name, FileEntry_t *f)
{
	memset(name,0,12*sizeof(char));
	for (int i = 0; i < 8; i++)
		if (f->name[i] == ' ') {
			name[i] = '\0';
			break;
		} else
			name[i] = f->name[i];

	if (f->extension[0] != ' ') {
		strcat(name, ".");
		int k = strlen(name);
		for (int i = 0; i < 3; i++)
			if (f->extension[i] == ' ') {
				name[i + k] = '\0';
				break;
			} else
				name[i + k] = f->extension[i];
	}
	name[11] = '\0';
}

bool fat_next_sector(int* next, int actual)
{
	*next = fat.entry[actual];
	if ((*next == 0) || (*next >= 0x0FF0))
		return false;
	else
		return true;
}

// start from the @cstart-th cluster, read the whole file to @buf
// return the number of bytes read
int fat12_read_clusters(char* buf, int cstart)
{
	
	int i, clust = cstart;
	for (i = 0; true; ++i)
	{
		printf("%d\n", clust);
		uintptr_t addr = (uintptr_t) buf + i * FAT_SECTOR_SIZE;
		// remember to recalculate cstart
		read_sectors(addr, clust + FAT_DATA_REGION_START - 2, 1);
		if (fat_next_sector(&clust, clust) == false)
			break;
	}
	return i;
}

int fat12_read_file(char* filename, char* buf)
{
	// char str_path[1024];
	// char* tmp = str_path, *filename;

	// // Copy the path string into a temporary area.
	// strncpy(str_path, path, sizeof(str_path));

	// do
	// {
	// 	filename = strsep(&tmp, "/");
	// } while (*filename == '\0');

	int size;
	bool flag = false;
	for (int i = 0; i < FAT_NUM_ROOT_ENTRY; ++i){
		char entry_name[12]; // with dot(.)!
		fat12_construct_file_name(entry_name,&root_dir.entry[i]);
		if (i < 25)
			printf("%s\n", entry_name);
		if (strcmp(filename,entry_name) != 0)
			continue;
		size = fat12_read_clusters(buf,root_dir.entry[i].startCluster);
		printf("Read in %d sectors!\n", size);
		flag = true;
		break;
	}

	if (!flag)
		put_error("FAT No this file!");

	return size;
}

#endif // FAT12_H