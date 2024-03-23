/*
 * ftps_file.c
 *
 *  Created on: Feb 18, 2020
 *      Author: Sander
 */

#include "ftp_file.h"
#include "ftp.h"

FRESULT ftps_f_stat(const char *path, FILINFO *nfo) {
    return f_stat(path, nfo);
}

FRESULT ftps_f_opendir(DIR *dir_p, const char *path) {
    return f_opendir(dir_p, path);
}

FRESULT ftps_f_readdir(DIR *dp, FILINFO *fno) { return f_readdir(dp, fno); }

FRESULT ftps_f_unlink(const char *path) { return f_unlink(path); }

FRESULT ftps_f_open(FIL *file_p, const char *path, uint8_t mode) {
    return f_open(file_p, path, mode);
}

FSIZE_t ftps_f_size(FIL *file_p) { return f_size(file_p); }

FRESULT ftps_f_close(FIL *file_p) { return f_close(file_p); }

FRESULT ftps_f_write(FIL *file_p, const void *buffer, uint32_t len,
                     UINT *written) {
    return f_write(file_p, buffer, len, written);
}

FRESULT ftps_f_read(FIL *file_p, void *buffer, uint32_t len, UINT *read) {
    return f_read(file_p, buffer, len, read);
}

FRESULT ftps_f_mkdir(const char *path) { return f_mkdir(path); }

FRESULT ftps_f_rename(const char *from, const char *to) {
    return f_rename(from, to);
}

FRESULT ftps_f_utime(const TCHAR *path, const FILINFO *fno) {
    /*
    return f_utime(path, fno);
    */
    return FR_OK;
}

FRESULT ftps_f_getfree(const TCHAR *path, DWORD *nclst, FATFS **fatfs) {
    return f_getfree(path, nclst, fatfs);
}
