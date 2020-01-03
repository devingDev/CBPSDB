/*
  MODIFIED FILE!
  This file was modified by coderx3, this is not the original file!

  VitaShell
  Copyright (C) 2015-2018, TheFloW

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef FILE_H
#define FILE_H

#include <psp2/types.h> 
#include <psp2/rtc.h> 
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#include "./strnatcmp.h"


#define MAX_DIR_LEVELS 128

#define PACKAGE_DIR "ux0:data/cbps/pkg"
//#define HEAD_BIN PACKAGE_DIR "/sce_sys/package/head.bin"
#define HOME_PATH "home"
#define DIR_UP ".."

#define SCE_ERROR_ERRNO_EEXIST 0x80010011
#define SCE_ERROR_ERRNO_ENODEV 0x80010013

#define MAX_PATH_LENGTH 1024
#define MAX_NAME_LENGTH 256
#define MAX_SHORT_NAME_LENGTH 64
#define MAX_MOUNT_POINT_LENGTH 16

#define DIRECTORY_SIZE (4 * 1024)
#define TRANSFER_SIZE (128 * 1024)
#define SYMLINK_HEADER_SIZE 4
#define SYMLINK_MAX_SIZE  (SYMLINK_HEADER_SIZE + MAX_PATH_LENGTH)
#define SYMLINK_EXT "lnk"

enum FileTypes {
  FILE_TYPE_UNKNOWN,
  FILE_TYPE_ARCHIVE,
  FILE_TYPE_BMP,
  FILE_TYPE_INI,
  FILE_TYPE_JPEG,
  FILE_TYPE_MP3,
  FILE_TYPE_MP4,
  FILE_TYPE_OGG,
  FILE_TYPE_PNG,
  FILE_TYPE_PSP2DMP,
  FILE_TYPE_SFO,
  FILE_TYPE_TXT,
  FILE_TYPE_VPK,
  FILE_TYPE_XML,
};

enum FileSortFlags {
  SORT_NONE,
  SORT_BY_NAME,
  SORT_BY_SIZE,
  SORT_BY_DATE,
};

enum FileMoveFlags {
  MOVE_INTEGRATE  = 0x1, // Integrate directories
  MOVE_REPLACE    = 0x2, // Replace files
};
typedef struct {
  int to_file; // 1: to file, 0: to directory
  char *target_path;
  int target_path_length;
} Symlink;

typedef struct FileListEntry {
  struct FileListEntry *next;
  struct FileListEntry *previous;
  char *name;
  int name_length;
  int is_folder;
  int type;
  int is_symlink;
  Symlink *symlink;
  SceOff size;
  SceOff size2;
  SceDateTime ctime;
  SceDateTime mtime;
  SceDateTime atime;
} FileListEntry;

typedef struct {
  FileListEntry *head;
  FileListEntry *tail;
  int length;
  char path[MAX_PATH_LENGTH];
  int files;
  int folders;
  int is_in_archive;
} FileList;

int removePath(const char *path);
void convertLocalTimeToUtc(SceDateTime *time_utc, SceDateTime *time_local);
int ReadFile(const char *file, void *buf, int size);
int hasEndSlash(const char *path);
int checkFileExist(const char *file);
int checkFolderExist(const char *folder);
int removeEndSlash(char *path);
int addEndSlash(char *path);
int getFileType(const char *file);
void fileListEmpty(FileList *list);
void fileListAddEntry(FileList *list, FileListEntry *entry, int sort);
int copyFile(const char *src_path, const char *dst_path);
int copyPath(const char *src_path, const char *dst_path);
#endif
