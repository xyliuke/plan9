//
// Created by liuke on 16/3/27.
//

#ifndef COMMON_COMPRESS_H
#define COMMON_COMPRESS_H

namespace plan9
{
    class compress_wrap {
    public:
        /**
         * 压缩数据
         * @param data 需要压缩的原始数据
         * @param len 需要压缩的原始数据的长度
         * @param ret 压缩后的数据
         * @return 返回压缩后的数据长度
         */
        static int compress(const char* data, int len, char* ret, int* ret_len);

        /**
         * 计算压缩后数据空间的大小,只是估算
         */
        static int maybe_compressed_size(int len);

        /**
         * 解压数据
         * @param data 需要解压的原始数据
         * @param len 需要解压的原始数据的长度
         * @param ret 解压后的数据
         * @return 返回解压后的数据长度
         */
        static int decompress(const char *data, int len, char *ret, int* ret_len);
    };

}


#endif //COMMON_COMPRESS_H
