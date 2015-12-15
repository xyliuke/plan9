//
// Created by liuke on 15/12/15.
//

#include <test/test_def.h>
#include <util/time.h>

#ifdef TEST_ENABLE

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

#endif