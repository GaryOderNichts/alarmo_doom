// This file has been taken https://github.com/ghidraninja/game-and-watch-doom
#include "ff.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

#define FID_DOOM 1

FRESULT f_open (FIL* fp, const char* path, char mode) {
    if(strcmp(path, "0:/doom/doom1.wad") == 0) {
        fp->fileid = FID_DOOM;
        fp->position = 0;
    }
    return FR_OK;
}

int f_tell(FIL *fp) {
    return 0;
}

int f_size(FIL *fp) {
    if(fp->fileid == FID_DOOM) {
        return doom1_wad_unc_size;
    }
    return 0;
}

FRESULT f_close (FIL* fp) {
    return FR_OK;
}
FRESULT f_read (FIL* fp, void* buff, unsigned btr, unsigned* br) {
    if(fp->fileid == FID_DOOM) {
        unsigned read_len = 0;
        if(doom1_wad_unc_size - fp->position < btr) {
            read_len = doom1_wad_unc_size - fp->position;
        } else {
            read_len = btr;
        }

        memcpy(buff, doom1_wad_unc + fp->position, read_len);
        *br = read_len;
        fp->position += read_len;
    }
    return FR_OK;
}
FRESULT f_readn (FIL* fp, void* buff, unsigned btr, unsigned* br) {
     if(fp->fileid == FID_DOOM) {
        unsigned read_len = 0;
        if(doom1_wad_unc_size - fp->position < btr) {
            read_len = doom1_wad_unc_size - fp->position;
        } else {
            read_len = btr;
        }

        memcpy(buff, doom1_wad_unc + fp->position, read_len);
        *br = read_len;
        fp->position += read_len;
    }
    return FR_OK;
}
FRESULT f_write (FIL* fp, const void* buff, unsigned btw, unsigned* bw) {
    return FR_OK;
}
FRESULT f_writen (FIL* fp, const void* buff, unsigned btw, unsigned* bw) {
    return FR_OK;
}
FRESULT f_rename (const char* path_old, const char* path_new) {
    return FR_OK;
}

FRESULT f_mkdir (const char* path) {
    return FR_OK;
}

FRESULT f_stat (const char* path, FILINFO* fno) {
    if(strcmp(path, "0:/doom/doom1.wad") == 0) {
        return FR_OK;
    }
    return -1;
}
FRESULT f_lseek (FIL* fp, unsigned ofs) {
    if(fp->fileid == FID_DOOM) {
        fp->position = ofs;
    }
    
    return FR_OK;
}
FRESULT f_unlink (const char* path) {
    return FR_OK;
}