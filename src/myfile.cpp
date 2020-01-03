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

#include <debugnet.h>
#include "./myfile.h"
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <psp2/rtc.h> 

#define MAX_PATH_LENGTH 1024
#define MAX_NAME_LENGTH 256
#define MAX_SHORT_NAME_LENGTH 64

#define TRANSFER_SIZE (128 * 1024)
#define SCE_ERROR_ERRNO_EEXIST 0x80010011
#define SCE_ERROR_ERRNO_ENODEV 0x80010013

typedef struct {
  char *extension;
  int type;
} ExtensionType;
static ExtensionType extension_types[] = {
  { ".7Z",       FILE_TYPE_ARCHIVE },
  { ".AR",       FILE_TYPE_ARCHIVE },
  { ".BMP",      FILE_TYPE_BMP },
  { ".BZ2",      FILE_TYPE_ARCHIVE },
  { ".CPIO",     FILE_TYPE_ARCHIVE },
  { ".GRZ",      FILE_TYPE_ARCHIVE },
  { ".GZ",       FILE_TYPE_ARCHIVE },
  { ".INI",      FILE_TYPE_INI },
  { ".ISO",      FILE_TYPE_ARCHIVE },
  { ".JPEG",     FILE_TYPE_JPEG },
  { ".JPG",      FILE_TYPE_JPEG },
  { ".LRZ",      FILE_TYPE_ARCHIVE },
  { ".LZ",       FILE_TYPE_ARCHIVE },
  { ".LZ4",      FILE_TYPE_ARCHIVE },
  { ".LZMA",     FILE_TYPE_ARCHIVE },
  { ".LZO",      FILE_TYPE_ARCHIVE },
  { ".MP3",      FILE_TYPE_MP3 },
  { ".MP4",      FILE_TYPE_MP4 },
  { ".MTREE",    FILE_TYPE_ARCHIVE },
  { ".OGG",      FILE_TYPE_OGG },
  { ".PNG",      FILE_TYPE_PNG },
  { ".PSARC",    FILE_TYPE_ARCHIVE },
  { ".PSP2ARC",  FILE_TYPE_ARCHIVE },
  { ".PSP2DMP",  FILE_TYPE_PSP2DMP },
  { ".RAR",      FILE_TYPE_ARCHIVE },
  { ".SFO",      FILE_TYPE_SFO },
  { ".SHAR",     FILE_TYPE_ARCHIVE },
  { ".TAR",      FILE_TYPE_ARCHIVE },
  { ".TAZ",      FILE_TYPE_ARCHIVE },
  { ".TBZ",      FILE_TYPE_ARCHIVE },
  { ".TBZ2",     FILE_TYPE_ARCHIVE },
  { ".TGZ",      FILE_TYPE_ARCHIVE },
  { ".TLZ",      FILE_TYPE_ARCHIVE },
  { ".TMP",      FILE_TYPE_PSP2DMP },
  { ".TXT",      FILE_TYPE_TXT },
  { ".TXZ",      FILE_TYPE_ARCHIVE },
  { ".TZ",       FILE_TYPE_ARCHIVE },
  { ".TZ2",      FILE_TYPE_ARCHIVE },
  { ".TZMA",     FILE_TYPE_ARCHIVE },
  { ".TZO",      FILE_TYPE_ARCHIVE },
  { ".TZST",     FILE_TYPE_ARCHIVE },
  { ".UU",       FILE_TYPE_ARCHIVE },
  { ".VPK",      FILE_TYPE_VPK },
  { ".WAR",      FILE_TYPE_ARCHIVE },
  { ".XAR",      FILE_TYPE_ARCHIVE },
  { ".XML",      FILE_TYPE_XML },
  { ".XZ",       FILE_TYPE_ARCHIVE },
  { ".Z",        FILE_TYPE_ARCHIVE },
  { ".ZIP",      FILE_TYPE_ARCHIVE },
  { ".ZST",      FILE_TYPE_ARCHIVE },
};

void convertLocalTimeToUtc(SceDateTime *time_utc, SceDateTime *time_local) {
  SceRtcTick tick;
  sceRtcGetTick(time_local, &tick);
  sceRtcConvertLocalTimeToUtc(&tick, &tick);
  sceRtcSetTick(time_utc, &tick);  
}


int hasEndSlash(const char *path) {
  return path[strlen(path) - 1] == '/';
}

int removeEndSlash(char *path) {
  int len = strlen(path);

  if (path[len - 1] == '/') {
    path[len - 1] = '\0';
    return 1;
  }

  return 0;
}

int addEndSlash(char *path) {
  int len = strlen(path);
  if (len < MAX_PATH_LENGTH - 2) {
    if (path[len - 1] != '/') {
      path[len] = '/';
      path[len + 1] = '\0';
      return 1;
    }
  }

  return 0;
}
int getFileType(const char *file) {
  char *p = strrchr(file, '.');
  if (p) {
    int i;
    for (i = 0; i < (sizeof(extension_types) / sizeof(ExtensionType)); i++) {
      if (strcasecmp(p, extension_types[i].extension) == 0) {
        return extension_types[i].type;
      }
    }
  }

  return FILE_TYPE_UNKNOWN;
}
int removePath(const char *path) {
  SceUID dfd = sceIoDopen(path);
  if (dfd >= 0) {
    int res = 0;

    do {
      SceIoDirent dir;
      memset(&dir, 0, sizeof(SceIoDirent));

      res = sceIoDread(dfd, &dir);
      if (res > 0) {
        char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
        snprintf(new_path, MAX_PATH_LENGTH, "%s%s%s", path, hasEndSlash(path) ? "" : "/", dir.d_name);

        if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
          int ret = removePath(new_path);
          if (ret <= 0) {
            free(new_path);
            sceIoDclose(dfd);
            return ret;
          }
        } else {
          int ret = sceIoRemove(new_path);
          if (ret < 0) {
            free(new_path);
            sceIoDclose(dfd);
            return ret;
          }

          /*cancel

              free(new_path);
              sceIoDclose(dfd);
              return 0;

          */
        }

        free(new_path);
      }
    } while (res > 0);

    sceIoDclose(dfd);

    int ret = sceIoRmdir(path);
    if (ret < 0)
      return ret;

    /* cancel
      return 0;
    */
  } else {
    int ret = sceIoRemove(path);
    if (ret < 0)
      return ret;

    /* cancel
      return 0;
    */
  }

  return 1;
}
void fileListAddEntry(FileList *list, FileListEntry *entry, int sort) {
  if (!list || !entry)
    return;

  entry->next = NULL;
  entry->previous = NULL;

  if (list->head == NULL) {
    list->head = entry;
    list->tail = entry;
  } else {
    if (sort != SORT_NONE) {
      FileListEntry *p = list->head;
      FileListEntry *previous = NULL;

      char entry_name[MAX_NAME_LENGTH];
      strcpy(entry_name, entry->name);
      removeEndSlash(entry_name);

      while (p) {
        char p_name[MAX_NAME_LENGTH];
        strcpy(p_name, p->name);
        removeEndSlash(p_name);

        // '..' is always at first
        if (strcmp(entry_name, "..") == 0)
          break;

        if (strcmp(p_name, "..") == 0) {
          previous = p;
          p = p->next;
          continue;
        }

        // Sort by type
        if (sort == SORT_BY_NAME) {
          // First folders then files
          if (entry->is_folder > p->is_folder)
            break;
        } else if (sort == SORT_BY_SIZE || sort == SORT_BY_DATE) {
          // First files then folders
          if (entry->is_folder < p->is_folder)
            break;
        }

        if (sort == SORT_BY_NAME) {
          // Sort by name within the same type
          if (entry->is_folder == p->is_folder) {
            if (strnatcasecmp(entry_name, p_name) < 0) {
              break;
            }
          }
        } else if (sort == SORT_BY_SIZE) {
          // Sort by name for folders
          if (entry->is_folder && p->is_folder) {
            if (strnatcasecmp(entry_name, p_name) < 0) {
              break;
            }
          } else if (!entry->is_folder && !p->is_folder) {
            // Sort by size for files
            if (entry->size > p->size)
              break;

            // Sort by name for files with the same size
            if (entry->size == p->size) {
              if (strnatcasecmp(entry_name, p_name) < 0) {
                break;
              }
            }
          }
        } else if (sort == SORT_BY_DATE) {
          if (entry->is_folder == p->is_folder) {
            SceRtcTick entry_tick, p_tick;
            sceRtcGetTick(&entry->mtime, &entry_tick);
            sceRtcGetTick(&p->mtime, &p_tick);

            // Sort by date within the same type
            if (entry_tick.tick > p_tick.tick)
              break;

            // Sort by name for files and folders with the same date
            if (entry_tick.tick == p_tick.tick) {
              if (strnatcasecmp(entry_name, p_name) < 0) {
                break;
              }
            }
          }
        }

        previous = p;
        p = p->next;
      }

      if (previous == NULL) { // Order: entry (new head) -> p (old head)
        entry->next = p;
        p->previous = entry;
        list->head = entry;
      } else if (previous->next == NULL) { // Order: p (old tail) -> entry (new tail)
        FileListEntry *tail = list->tail;
        tail->next = entry;
        entry->previous = tail;
        list->tail = entry;
      } else { // Order: previous -> entry -> p
        previous->next = entry;
        entry->previous = previous;
        entry->next = p;
        p->previous = entry;
      }
    } else {
      FileListEntry *tail = list->tail;
      tail->next = entry;
      entry->previous = tail;
      list->tail = entry;
    }
  }

  list->length++;
}
void fileListEmpty(FileList *list) {
  if (!list)
    return;

  FileListEntry *entry = list->head;

  while (entry) {
    FileListEntry *next = entry->next;
    free(entry->name);
    free(entry);
    entry = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->length = 0;
  list->files = 0;
  list->folders = 0;
}

int copyFile(const char *src_path, const char *dst_path) {
  // The source and destination paths are identical
  if (strcasecmp(src_path, dst_path) == 0) {
    return -1;
  }

  // The destination is a subfolder of the source folder
  int len = strlen(src_path);
  if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/')) {
    return -2;
  }

  SceUID fdsrc = sceIoOpen(src_path, SCE_O_RDONLY, 0);
  if (fdsrc < 0){

    //debugNetPrintf(DEBUG,"Error copying file, couldn't open source file  %s \r\n"  , src_path);
    return fdsrc;
  }
  SceUID fddst = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
  if (fddst < 0) {
    //debugNetPrintf(DEBUG,"Error copying file, couldn't open destination file  %s \r\n" , dst_path);
    sceIoClose(fdsrc);
    return fddst;
  }

  void *buf = memalign(4096, TRANSFER_SIZE);

  while (1) {
    int read = sceIoRead(fdsrc, buf, TRANSFER_SIZE);

    if (read < 0) {
      //debugNetPrintf(DEBUG,"Error reading from  src  file  %s \r\n" , src_path );
      free(buf);

      sceIoClose(fddst);
      sceIoClose(fdsrc);

      sceIoRemove(dst_path);

      return read;
    }

    if (read == 0)
      break;

    int written = sceIoWrite(fddst, buf, read);

    if (written < 0) {
      //debugNetPrintf(DEBUG,"Error writing to dst file %s \r\n" , dst_path);
      free(buf);

      sceIoClose(fddst);
      sceIoClose(fdsrc);

      sceIoRemove(dst_path);

      return written;
    }


  }

  free(buf);

  // Inherit file stat
  SceIoStat stat;
  memset(&stat, 0, sizeof(SceIoStat));
  sceIoGetstatByFd(fdsrc, &stat);
  sceIoChstatByFd(fddst, &stat, 0x3B);

  sceIoClose(fddst);
  sceIoClose(fdsrc);

  return 1;
}

int checkFileExist(const char *file) {
  SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
  if (fd < 0)
    return 0;

  sceIoClose(fd);
  return 1;
}

int checkFolderExist(const char *folder) {
  SceUID dfd = sceIoDopen(folder);
  if (dfd < 0)
    return 0;

  sceIoDclose(dfd);
  return 1;
}

int ReadFile(const char *file, void *buf, int size) {
  SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
  if (fd < 0)
    return fd;

  int read = sceIoRead(fd, buf, size);

  sceIoClose(fd);
  return read;
}

int copyPath(const char *src_path, const char *dst_path) {
  // The source and destination paths are identical
  if (strcasecmp(src_path, dst_path) == 0) {
    return -1;
  }

  // The destination is a subfolder of the source folder
  int len = strlen(src_path);
  if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/')) {
    return -2;
  }

  SceUID dfd = sceIoDopen(src_path);
  if (dfd >= 0) {
    SceIoStat stat;
    memset(&stat, 0, sizeof(SceIoStat));
    sceIoGetstatByFd(dfd, &stat);

    stat.st_mode |= SCE_S_IWUSR;

    int ret = sceIoMkdir(dst_path, 0777);
    if (ret < 0 && ret != SCE_ERROR_ERRNO_EEXIST) {
      //debugNetPrintf(DEBUG,"Error copying folder, couldn't make dir %s \r\n" , dst_path);
      sceIoDclose(dfd);
      return ret;
    }

    if (ret == SCE_ERROR_ERRNO_EEXIST) {
      sceIoChstat(dst_path, &stat, 0x3B);
    }

    int res = 0;

    do {
      SceIoDirent dir;
      memset(&dir, 0, sizeof(SceIoDirent));

      res = sceIoDread(dfd, &dir);
      if (res > 0) {
        char *new_src_path = malloc(strlen(src_path) + strlen(dir.d_name) + 2);
        snprintf(new_src_path, MAX_PATH_LENGTH - 1, "%s%s%s", src_path, hasEndSlash(src_path) ? "" : "/", dir.d_name);

        char *new_dst_path = malloc(strlen(dst_path) + strlen(dir.d_name) + 2);
        snprintf(new_dst_path, MAX_PATH_LENGTH - 1, "%s%s%s", dst_path, hasEndSlash(dst_path) ? "" : "/", dir.d_name);

        int ret = 0;

        if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
          ret = copyPath(new_src_path, new_dst_path);
        } else {
          ret = copyFile(new_src_path, new_dst_path);
        }

        free(new_dst_path);
        free(new_src_path);

        if (ret <= 0) {
          sceIoDclose(dfd);
          return ret;
        }
      }
    } while (res > 0);

    sceIoDclose(dfd);
  } else {
    return copyFile(src_path, dst_path);
  }

  return 1;
}
