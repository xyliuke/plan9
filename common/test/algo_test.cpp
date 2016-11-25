//
// Created by liuke on 16/3/27.
//


#include <util/util.h>
#include <util/time.h>
#include "test/test_def.h"

#ifdef ALGO_TEST

#include "algorithm/compress.h"
#include "algorithm/crypto_wrap.h"

TEST(algo, compress) {
//    int ret_len = 1000;
//    char ret[ret_len];
//
//    int ret_len_1 = 1000;
//    char ret_1[ret_len_1];
//
//
//
//    long long int s = plan9::time::microseconds();
//    std::string src = "hello";
////    std::string src = "{\"args\":{\"server\":0,\"test\":\"hello world from ios\",\"timeout\":5000},\"aux\":{\"action\":\"callback\",\"from\":[\"ID-OTHER-1459089065999386-9950-AL\"],\"id\":\"ID-OTHER-1459089065999386-9950-AL\",\"to\":\"function\"}}";
//    for (int i = 0; i < 1; ++i) {
//        int len = plan9::compress_wrap::compress(src.c_str(), src.length(), ret, &ret_len);
//        std::cout << "compress data len : " << len << std::endl;
//        std::cout << "compress data : " << plan9::util::instance().char_to_string(ret, len) << std::endl;
//        int len_1 = plan9::compress_wrap::decompress(ret, ret_len, ret_1, &ret_len_1);
//    }
//
//    std::cout << "maybe compress size " << plan9::compress_wrap::maybe_compressed_size(1000) << std::endl;
//
//    int t = plan9::time::microseconds() - s;
//    std::cout << "compress time : " << t << std::endl;



}

TEST(algo, compress_file) {
//    char code[7] = "123456";
//    plan9::compress_wrap::compress_zip("./data", code, "./11.zip");
//    plan9::compress_wrap::decompress_zip("./11.zip", code, "/Users/liuke/Desktop/ttt");
//    plan9::compress_wrap::get_file_crc32("/Users/liuke/Desktop/1024.png");
}

TEST(algo, crypt) {
    using namespace plan9;
    std::string s1 = "abc";
    std::cout << s1 << " md5 " << crypto_wrap::MD5(s1) << std::endl;

    std::cout << s1 << " md5 file " << crypto_wrap::MD5_file("./abc.zip") << std::endl;
    std::cout << s1 << " md5 file " << crypto_wrap::MD5_file("./abc1.zip") << std::endl;

    std::cout << s1 << " sha1 " << crypto_wrap::SHA1(s1) << std::endl;
    std::cout << s1 << " sha1 file " << crypto_wrap::SHA1_file("./abc.zip") << std::endl;

    std::cout << s1 << " base64 " << crypto_wrap::base64(s1) << std::endl;
    std::cout << s1 << " base64 decode " << crypto_wrap::base64_decode(crypto_wrap::base64(s1)) << std::endl;
}

#endif