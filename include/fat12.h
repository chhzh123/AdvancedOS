// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** FAT12.h ******/

#ifndef FAT12_H
#define FAT12_H

#include "ide.h"
#include "stdio.h"
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
#define FAT_NUM_DIR_ENTRY 16

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

char curr_path[50];
int curr_dir;

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

typedef struct fat12_file_attr
{
	unsigned long read_only : 1;
	unsigned long hidden    : 1;
	unsigned long system    : 1;
	unsigned long label	    : 1;
	unsigned long directory : 1;
	unsigned long archive   : 1;
	unsigned long __res     : 2;
} __attribute__ ((packed)) fat12_file_attr_t;

// Root Directory
// 16 directory entries per sector = 512B / 32B = 16
// 14 sectors * 16 = 224 entries
typedef struct FileEntry // 32B
{
	char name[8];
	char extension[3];
	fat12_file_attr_t attribute;
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

typedef struct root_directory
{
	FileEntry_t entry[FAT_SECTOR_SIZE / sizeof(FileEntry_t) * FAT_ROOT_SIZE]; // 224

} __attribute__ ((packed)) fat_root_t;
static fat_root_t root_dir;

typedef struct sub_directory
{
	FileEntry_t entry[FAT_SECTOR_SIZE / sizeof(FileEntry_t)]; // 16

} __attribute__ ((packed)) fat_subdir_t;
static fat_subdir_t subdir;

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

	// root directory
	read_sectors((uintptr_t)&root_dir,FAT_ROOT_REGION_START,FAT_ROOT_SIZE);

	// set current path
	curr_dir = FAT_ROOT_REGION_START;
	strcpy(curr_path,"/");

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

char* fat12_construct_file_name(char* name, FileEntry_t *f)
{
	memset(name,0,13*sizeof(char));
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
	name[12] = '\0';
	tolower(name);
	return name;
}

bool fat12_next_sector(uint32_t* next, uint32_t actual)
{
	*next = fat.entry[actual];
	if ((*next == 0) || (*next >= 0x0FF0))
		return false;
	else
		return true;
}

// start from the @cstart-th cluster, read the whole file to @buf
// return the number of bytes read
int fat12_read_clusters(char* buf, uint32_t cstart)
{

	int i;
	uint32_t clust = cstart;
	for (i = 0; true; ++i)
	{
#ifdef DEBUG
		printf("Read cluster: %d\n", clust);
#endif
		uintptr_t addr = (uintptr_t) buf + i * FAT_SECTOR_SIZE;
		// remember to recalculate cstart
		read_sectors(addr, clust + FAT_DATA_REGION_START - 2, 1);
		enable();
		if (fat12_next_sector(&clust, clust) == false)
			break;
	}
	return i;
}

bool fat12_read_file(char* filename, char* addr)
{
#ifdef DEBUG
	printf("%s\n", filename);
#endif
	for (int i = 0; i < FAT_NUM_ROOT_ENTRY; ++i){
		char entry_name[13]; // with dot(.)!
		fat12_construct_file_name(entry_name,&root_dir.entry[i]);
		if (strcmp(filename,entry_name) != 0)
			continue;
		fat12_read_clusters(addr,root_dir.entry[i].startCluster);
		return true;
	}

	put_error("FAT No this file!");

	return false;
}

 void fat12_show_file_attrib(fat12_file_attr_t attr)
{
	if (attr.label)
	{
		printf("<label> ");
		return;
	}
	printf("%c", attr.directory	? 'd' : '-');
	printf("%c", attr.read_only	? 'r' : '-');
	printf("%c", attr.hidden	? 'h' : '-');
	printf("%c", attr.system	? 's' : '-');
	printf("%c", attr.archive	? 'a' : '-');
}

int fat12_get_num_cluster(uint32_t start)
{
	uint32_t c = 1, cl;

	// The root directory (start=0) has a fixed size.
	if (!start)
	{
		return ((bootsector.BPB_RootDirectoryEntries * sizeof(FileEntry_t))
			/ FAT_SECTOR_SIZE);
	}

	// Calculate the size of the directory... it's not the root!
	cl = start;
	while (fat12_next_sector(&cl, cl))
		c++;

	return c;
}

bool fat12_show_file_entry(FileEntry_t *f)
{
	fat_date_t FileDate;
	fat_time_t FileTime;
	char name[13];

	// Do not print the disk label & deleted files
	if (!(f->attribute.label) && strlen(f->name) != 0 &&
		(((unsigned char)f->name[0] & 0x000000E5) != 0xE5))
	{
		int_to_date(&FileDate, f->date);
		int_to_time(&FileTime, f->time);

		// Print attribute informations.
		fat12_show_file_attrib(f->attribute);

		// Print the file date and time.
		printf("  %04d/%02d/%02d", (FileDate.year+1980), FileDate.month, FileDate.day);
		printf("  %02d:%02d:%02d", FileTime.hour, FileTime.minute, FileTime.second);

		// Print the file size.
		if (f->attribute.directory)
			printf("  %10d", fat12_get_num_cluster(f->startCluster) * FAT_SECTOR_SIZE);
		else
			printf("  %10d", (f->fileLength));

		// Print the filename.
		if (f->attribute.directory)
			set_color(LIGHT_BLUE,BLACK);
		if (f->attribute.hidden)
			set_color(DARK_GREY,BLACK);
		if (f->attribute.system)
			set_color(MAGENTA,BLACK);
		printf("  %s\n", fat12_construct_file_name(name, f));
		set_color(WHITE,BLACK);

		return true;
	}
	return false;
}

void fat12_ls()
{
	if (curr_dir == FAT_ROOT_REGION_START)
		for (int i = 0; i < FAT_NUM_ROOT_ENTRY; ++i)
			fat12_show_file_entry(&root_dir.entry[i]);
	else
		for (int i = 0; i < FAT_NUM_DIR_ENTRY; ++i)
			fat12_show_file_entry(&subdir.entry[i]);
}

void fat12_set_dir(int new_dir, char* action)
{
	if (curr_dir == FAT_ROOT_REGION_START){
		curr_dir = new_dir;
		strcpy(curr_path,"/");
		strcat(curr_path,action);
	} else {

		if (new_dir == 0)
			new_dir = FAT_ROOT_REGION_START;
		curr_dir = new_dir;

		if (strcmp(action,".") == 0)
			; // do nothing
		else if (strcmp(action,"..") == 0) {
			reverse(curr_path);
			char* tmp_path = curr_path;
			strsep(&tmp_path,"/");
			reverse(tmp_path);
			strcpy(curr_path,tmp_path);
			if (curr_dir == FAT_ROOT_REGION_START)
				strcpy(curr_path,"/");
		} else {
			strcat(curr_path,"/");
			strcat(curr_path,action);
		}
	}
}

FileEntry_t* fat12_find_entry(char* folder)
{
	if (curr_dir == FAT_ROOT_REGION_START){
		for (int i = 0; i < FAT_NUM_ROOT_ENTRY; ++i){
			char entry_name[13]; // with dot(.)!
			fat12_construct_file_name(entry_name,&root_dir.entry[i]);
			if (!(root_dir.entry[i].attribute.directory && strcmp(folder,entry_name) == 0))
				continue;
			return &root_dir.entry[i];
		}
	} else {
		fat12_read_clusters((char*)&subdir,curr_dir);
		for (int i = 0; i < FAT_NUM_DIR_ENTRY; ++i){
			char entry_name[13]; // with dot(.)!
			fat12_construct_file_name(entry_name,&subdir.entry[i]);
			if (!(subdir.entry[i].attribute.directory && strcmp(folder,entry_name) == 0))
				continue;
			return &subdir.entry[i];
		}
	}
	return NULL;
}

bool fat12_cd(char* folder)
{
	FileEntry_t* fe = fat12_find_entry(folder);
	if (fe == NULL){
		put_error("No such file or directory!");
		return false;
	}
	fat12_set_dir(fe->startCluster,folder);
	fat12_read_clusters((char*)&subdir,fe->startCluster);
	for (int i = 0; i < FAT_NUM_DIR_ENTRY; ++i)
		fat12_show_file_entry(&subdir.entry[i]);
	return true;
}

#endif // FAT12_H