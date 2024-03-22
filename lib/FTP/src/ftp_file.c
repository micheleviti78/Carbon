/*
 * ftps_file.c
 *
 *  Created on: Feb 18, 2020
 *      Author: Sander
 */

#include "ftp_file.h"
#include "ftp.h"

#include <carbon/diag.hpp>

FRESULT ftps_f_stat(const char *path, FILINFO *nfo) {
    FRESULT res = f_stat(path, nfo);
    if (res != FR_OK) {
        DIAG(FTP "error %d retrieve info for file %s", res, path);
    }
    return res;
}

FRESULT ftps_f_opendir(DIR *dir_p, const char *path) {
    FRESULT res = f_opendir(dir_p, path);
    if (res != FR_OK) {
        DIAG(FTP "error %d opening directory %s", res, path);
    }
    return res;
}

FRESULT ftps_f_readdir(DIR *dp, FILINFO *fno) {
    FRESULT res = f_readdir(dp, fno);
    if (res != FR_OK) {
        DIAG(FTP "error %d reading directory", res);
    }
    return res;
}

FRESULT ftps_f_unlink(const char *path) {
    FRESULT res = f_unlink(path);
    if (res != FR_OK) {
        DIAG(FTP "error %d unlinking %s", res, path);
    }
    return res;
}

FRESULT ftps_f_open(FIL *file_p, const char *path, uint8_t mode) {
    FRESULT res = f_open(file_p, path, mode);
    if (res != FR_OK) {
        DIAG(FTP "error %d opening file %s with mode %u", res, path, mode);
    }
    return res;
}

FSIZE_t ftps_f_size(FIL *file_p) { return f_size(file_p); }

FRESULT ftps_f_close(FIL *file_p) {
    FRESULT res = f_close(file_p);
    if (res != FR_OK) {
        DIAG(FTP "error %d closing file", res);
    }
    return res;
}

FRESULT ftps_f_write(FIL *file_p, const void *buffer, uint32_t len,
                     UINT *written) {
    FRESULT res = f_write(file_p, buffer, len, written);
    if (res != FR_OK) {
        DIAG(FTP "error %d writing file", res);
    }
    return res;
}

FRESULT ftps_f_read(FIL *file_p, void *buffer, uint32_t len, UINT *read) {
    FRESULT res = f_read(file_p, buffer, len, read);
    if (res != FR_OK) {
        DIAG(FTP "error %d writing file", res);
    }
    return res;
}

FRESULT ftps_f_mkdir(const char *path) {
    FRESULT res = f_mkdir(path);
    if (res != FR_OK) {
        DIAG(FTP "error %d creating dir %s", res, path);
    }
    return res;
}

FRESULT ftps_f_rename(const char *from, const char *to) {
    FRESULT res = f_rename(from, to);
    if (res != FR_OK) {
        DIAG(FTP "error %d renaming %s to %s", res, from, to);
    }
    return res;
}

FRESULT ftps_f_utime(const TCHAR *path, const FILINFO *fno) {
    /*
    return f_utime(path, fno);
    */
    DIAG(FTP "ftps_f_utime not implemented");
    return FR_OK;
}

FRESULT ftps_f_getfree(const TCHAR *path, DWORD *nclst, FATFS **fatfs) {
    FRESULT res = f_getfree(path, nclst, fatfs);
    if (res != FR_OK) {
        DIAG(FTP "error %d f_getfree", res);
    }
    return res;
}
