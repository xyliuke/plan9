//#include <iostream>
#include "test/test_def.h"

#ifndef TEST_ENABLE
#import "algorithm/compress.h"
#endif

using namespace std;

int main(int argc, char** argv) {

    TEST_INIT
#ifndef TEST_ENABLE
    plan9::compress_wrap::deal_with_compress(argc, argv);
#endif

    return TEST_RUN;
}