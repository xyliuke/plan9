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
//    std::string src = "hello worldhello worldhello worldhello worldhello worldhello worldhello worldhello worldhello worldhello worldhello world";
    std::string src = "{\"args\":{\"server\":0,\"test\":\"hello world from ios\",\"timeout\":5000},\"aux\":{\"action\":\"callback\",\"from\":[\"ID-OTHER-1459089065999386-9950-AL\"],\"id\":\"ID-OTHER-1459089065999386-9950-AL\",\"to\":\"function\"}}";
    for (int i = 0; i < 1; ++i) {
        int len = plan9::compress_wrap::compress(src.c_str(), src.length(), ret, &ret_len);
//        std::cout << "compress data len : " << len << std::endl;
        int len_1 = plan9::compress_wrap::decompress(ret, ret_len, ret_1, &ret_len_1);
    }

    std::cout << "maybe compress size " << plan9::compress_wrap::maybe_compressed_size(1000) << std::endl;

    int t = plan9::time::microseconds() - s;
    std::cout << "compress time : " << t << std::endl;



}


#endif