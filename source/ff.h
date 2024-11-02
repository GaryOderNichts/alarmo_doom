// This file has been taken from https://github.com/ghidraninja/game-and-watch-doom
#ifndef _FF_H_
#define _FF_H_

// Custom implementation of 'ff' fat driver, but instead of fat we're reading data contained in the bin.

typedef int FRESULT;

#define FA_OPEN_EXISTING 1
#define FA_READ 2
#define FR_OK 0
#define FA_CREATE_ALWAYS 3
#define FA_WRITE 4
#define FR_EXIST 5

typedef int FILINFO;
typedef struct {
    unsigned fileid;
    unsigned position;
} FIL;

FRESULT f_open (FIL* fp, const char* path, char mode);				/* Open or create a file */
int f_tell(FIL *fp);
int f_size(FIL *fp);

FRESULT f_close (FIL* fp);											/* Close an open file object */
FRESULT f_read (FIL* fp, void* buff, unsigned btr, unsigned* br);			/* Read data from a file */
FRESULT f_readn (FIL* fp, void* buff, unsigned btr, unsigned* br);			/* Read data from a file without size limit */
FRESULT f_write (FIL* fp, const void* buff, unsigned btw, unsigned* bw);	/* Write data to a file */
FRESULT f_writen (FIL* fp, const void* buff, unsigned btw, unsigned* bw);	/* Write data to a file without size limit */
FRESULT f_rename (const char* path_old, const char* path_new);	/* Rename/Move a file or directory */
FRESULT f_mkdir (const char* path);
FRESULT f_stat (const char* path, FILINFO* fno);
FRESULT f_lseek (FIL* fp, unsigned ofs);								/* Move file pointer of a file object */
FRESULT f_unlink (const char* path);
#endif
