//
// Created by liuke on 16/3/27.
//


#include <util/util.h>
#include <util/time.h>
#include "test/test_def.h"

#ifdef ALGO_TEST

#include "algorithm/compress.h"

TEST(algo, compress) {
    int ret_len = 1000;
    char ret[ret_len];

    int ret_len_1 = 1000;
    char ret_1[ret_len_1];



    long long int s = plan9::time::microseconds();
    std::string src = "hello";
//    std::string src = "{\"args\":{\"server\":0,\"test\":\"hello world from ios\",\"timeout\":5000},\"aux\":{\"action\":\"callback\",\"from\":[\"ID-OTHER-1459089065999386-9950-AL\"],\"id\":\"ID-OTHER-1459089065999386-9950-AL\",\"to\":\"function\"}}";
    for (int i = 0; i < 1; ++i) {
        int len = plan9::compress_wrap::compress(src.c_str(), src.length(), ret, &ret_len);
        std::cout << "compress data len : " << len << std::endl;
        std::cout << "compress data : " << plan9::util::instance().char_to_string(ret, len) << std::endl;
        int len_1 = plan9::compress_wrap::decompress(ret, ret_len, ret_1, &ret_len_1);
    }

    std::cout << "maybe compress size " << plan9::compress_wrap::maybe_compressed_size(1000) << std::endl;

    int t = plan9::time::microseconds() - s;
    std::cout << "compress time : " << t << std::endl;



}

TEST(algo, compress_file) {
//    char code[7] = "123456";
//    plan9::compress_wrap::compress_zip("./data", code, "./11.zip");
//    plan9::compress_wrap::decompress_zip("./11.zip", code, "/Users/liuke/Desktop/ttt");
//    plan9::compress_wrap::get_file_crc32("/Users/liuke/Desktop/1024.png");
}


#endif