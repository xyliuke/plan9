//
//  zlib_wrap.cpp
//  anet
//
//  Created by ke liu on 08/11/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#include "zlib_wrap.hpp"
#include "zlib.h"

namespace plan9 {
    unsigned long zlib_wrap::zip(char *data, unsigned long len, char *dst_data, unsigned long dst_len) {
        unsigned long ret = dst_len;
        int result = compress2((unsigned char *)dst_data, &ret, (unsigned char*)data, len, Z_BEST_COMPRESSION);
        if (result == Z_OK) {
            return ret;
        }
        return 0;
    }
    
    unsigned long zlib_wrap::unzip(char *data, unsigned long len, char *dst_data, unsigned long dst_len) {
        unsigned long ret = dst_len;
        int result = uncompress((unsigned char*)dst_data, &ret, (unsigned char*)data, len);
        if (result == Z_OK) {
            return ret;
        }
        return 0;
    }

    char* zlib_wrap::unzip(char *data, unsigned long len, unsigned long *dst_len) {
        if (len > 0) {
            char* ret = (char*) malloc(len * 10);
            *dst_len = unzip(data, len, ret, len * 10);
            if (*dst_len > 0) {
                ret = (char*)realloc(ret, *dst_len);
                return ret;
            }
            free(ret);
        }
        return nullptr;
    }

    unsigned long zlib_wrap::gzip(char *data, unsigned long len, char *dst_data, unsigned long dst_len) {
        z_stream c_stream;
        int ret = dst_len;
        if (data && len > 0) {
            c_stream.zalloc = NULL;
            c_stream.zfree = NULL;
            c_stream.opaque = NULL;
            //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
            if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                    MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
                return 0;
            }
            c_stream.next_in = (unsigned char*)data;
            c_stream.avail_in = len;
            c_stream.next_out = (unsigned char*)dst_data;
            c_stream.avail_out = ret;
            while (c_stream.avail_in != 0 && c_stream.total_out < dst_len) {
                if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return 0;
            }
            if (c_stream.avail_in != 0) return c_stream.avail_in;
            for (;;) {
                int result = deflate(&c_stream, Z_FINISH);
                if (result == Z_STREAM_END) break;
                if (result != Z_OK) return 0;
            }
            if (deflateEnd(&c_stream) != Z_OK) return 0;
            ret = c_stream.total_out;
            return ret;
        }
        return 0;
    }

    unsigned long zlib_wrap::ungzip(char *data, unsigned long len, char *dst_data, unsigned long dst_len) {
        unsigned long ret = dst_len;
        z_stream d_stream = { 0 }; /* decompression stream */
        static char dummy_head[2] = {
                0x8 + 0x7 * 0x10,
                (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
        };
        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.opaque = NULL;
        d_stream.next_in = (unsigned char*)data;
        d_stream.avail_in = len;
        d_stream.avail_out = dst_len;
        d_stream.next_out = (unsigned char*)dst_data;
        //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
        if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return 0;
        //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
        while (d_stream.total_out < len && d_stream.total_in < dst_len) {
            int err = inflate(&d_stream, Z_NO_FLUSH);
            if (err == Z_STREAM_END) break;
            if (err != Z_OK) {
                if (err == Z_DATA_ERROR) {
                    d_stream.next_in = (Bytef*)dummy_head;
                    d_stream.avail_in = sizeof(dummy_head);
                    if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                        return 0;
                    }
                }
                else return 0;
            }
        }
        if (inflateEnd(&d_stream) != Z_OK) return 0;
        ret = d_stream.total_out;
        return ret;
    }

    char* zlib_wrap::ungzip(char *data, unsigned long len, unsigned long *dst_len) {
        if (len > 0) {
            char* ret = (char*) malloc(len * 10);
            *dst_len = ungzip(data, len, ret, len * 10);
            if (*dst_len > 0) {
                ret = (char*)realloc(ret, *dst_len);
                return ret;
            }
            free(ret);
        }
        return nullptr;
    }
}
