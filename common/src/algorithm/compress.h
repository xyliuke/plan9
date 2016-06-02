//
// Created by liuke on 16/3/27.
//

#ifndef COMMON_COMPRESS_H
#define COMMON_COMPRESS_H

#include <string>

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

        /**
         *  压缩目录
         *  @param path 需要压缩的文件目录,会将这个目录下所有文件进行递归压缩
         *  @param code 加密密码
         *  @param out_file 输出文件目录
         */
        static bool compress_zip(std::string path, const char *code, std::string out_zip_file);

        /**
         * 解压文件
         * @param file 需要解压的文件
         * @param code 解密密码
         * @param out_path 解压目录
         */
        static bool decompress_zip(std::string zip_file, const char* code, std::string out_path);

        /**
         * 获取文件的crc32检验值
         */
        static unsigned long get_file_crc32(std::string file);

        static void deal_with_compress(int argc, char** argv);
    };

}


#endif //COMMON_COMPRESS_H
