
#include "log.hpp"
#include <psp2/types.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/io/dirent.h>

SceUID fd;
void openLog(){
	fd = sceIoOpen("ux0:data/cbps/log.txt", SCE_O_WRONLY|SCE_O_APPEND|SCE_O_CREAT, 0777);
}
void writeLog(std::string data){
	const char * nl = "\r\n";
	const char * datac = data.c_str();
	sceIoWrite(fd, datac, strlen(datac));
	sceIoWrite(fd, nl, strlen(nl));
}
void closeLog(){
	sceIoClose(fd);
}