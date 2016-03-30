//
// Created by liuke on 16/3/27.
//

#include <algorithm>
#include "compress.h"
#include <zlib.h>

namespace plan9
{
    int compress_wrap::compress(const char *data, int len, char *ret, int* ret_len) {
        unsigned long tmp = (unsigned long)(*ret_len);
        int result = compress2((unsigned char *)ret, &tmp, (unsigned char*)data, len, Z_BEST_COMPRESSION);
        *ret_len = (int)tmp;
        if (result == Z_OK) {
            return *ret_len;
        }
        return -1;
    }

    int compress_wrap::maybe_compressed_size(int len) {
        return (int)compressBound(len);
    }

    int compress_wrap::decompress(const char *data, int len, char *ret, int* ret_len) {
        unsigned long tmp = (unsigned long)(*ret_len);
        int result = uncompress((unsigned char*)ret, &tmp, (unsigned char*)data, len);
        *ret_len = (int)tmp;
        if (result == Z_OK) {
            return *ret_len;
        }
        return -1;
    }
}