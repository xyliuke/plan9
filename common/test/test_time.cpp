//
// Created by liuke on 15/12/15.
//

#include <test/test_def.h>



#ifdef TEST_ENABLE

#include <util/time.h>


using namespace plan9;

TEST(time_test, current_time) {
    std::cout << "current data time1 :" << time::current_data_time() << std::endl;
    std::cout << "current data time2 :" << time::current_data_time() << std::endl;
    std::cout << "current data time3 :" << time::current_data_time() << std::endl;

    std::cout << "current data1 :" << time::current_data() << std::endl;
    std::cout << "current data2 :" << time::current_data() << std::endl;
    std::cout << "current data3 :" << time::current_data() << std::endl;
}


TEST(time_test, microsecond) {
    long long int micro = time::microseconds();
    long milli = time::milliseconds();
    long sec = time::seconds();
    std::cout << "micro:"<< micro << ",\tmili:" << milli << ",\tsecond:" << sec << std::endl;
    EXPECT_EQ(micro /1000, milli);
    EXPECT_EQ(milli /1000, sec);
}


TEST(time_test, performance) {
//    for (int i = 0; i < 1000000; ++i) {
//        int a = time::day();
//    }
}


TEST(time_test, mod) {
//    for (int i = 0; i < 1000000; ++i) {
//        int a = i % 10;
//    }
//    unsigned int i = 1;
//    while (true) {
//        i ++;
//        if (i == 0) {
//            std::cout << "int over" << std::endl;
//            break;
//        }
//    }
}

#endif