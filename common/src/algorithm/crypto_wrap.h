//
// Created by liuke on 23/11/2016.
//

#ifndef COMMON_CRYPTO_WARP_H
#define COMMON_CRYPTO_WARP_H

#include <string>

namespace plan9
{
    class crypto_wrap {
    public:
        /**
         * 计算字符串的md5散列值
         * @param text
         * @return
         */
        static std::string MD5(std::string text);
        /**
         * 对文件进行计算md5
         * @param file 文件路径，如果文件不存在，则返回空字符串
         * @return
         */
        static std::string MD5_file(std::string file);

        static std::string SHA1(std::string text);
        static std::string SHA1_file(std::string file);

        static std::string base64(std::string text);
        static std::string base64_decode(std::string text);

    };
}


#endif //COMMON_CRYPTO_WARP_H
