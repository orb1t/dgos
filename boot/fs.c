#include "fs.h"

fs_api_t fs_api;

int boot_open(const char *filename)
{
    return fs_api.boot_open(filename);
}

int boot_close(int file)
{
    return fs_api.boot_close(file);
}

int boot_pread(int file, void *buf, size_t bytes, off_t ofs)
{
    return fs_api.boot_pread(file, buf, bytes, ofs);
}