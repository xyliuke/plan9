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

        static std::string SHA256(std::string text);
        static std::string SHA256_file(std::string file);

        static std::string SHA512(std::string text);
        static std::string SHA512_file(std::string file);

        static std::string base64(std::string text);
        static std::string base64_decode(std::string text);

        static std::string rsa_public_key_file_encrypt(std::string key_file, std::string text, bool *error);
        static std::string rsa_public_key_file_decrypt(std::string key_file, std::string text, bool *error);
        static std::string rsa_private_key_file_decrypt(std::string key_file, std::string text, bool *error);
        static std::string rsa_private_key_file_encrypt(std::string key_file, std::string text, bool *error);

        static std::string rsa_public_key_encrypt(std::string key_content, std::string text, bool *error);
        static std::string rsa_public_key_decrypt(std::string key_content, std::string text, bool *error);
        static std::string rsa_private_key_decrypt(std::string key_content, std::string text, bool *error);
        static std::string rsa_private_key_encrypt(std::string key_content, std::string text, bool *error);

        static bool create_rsa_private_public_key_file(std::string private_key_file, std::string public_key_file, int length = 1024);
        static bool create_rsa_private_public_key(char* private_key, int* private_key_len, char* public_key, int* public_key_len, int length = 1024);
    };
}


#endif //COMMON_CRYPTO_WARP_H
