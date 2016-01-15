//
// Created by liuke on 15/12/15.
//

#include <test/test_def.h>



#ifdef TIME_TEST

#include <util/time.h>


using namespace plan9;

TEST(time_test, current_time) {
    std::string t = time::current_data_time();
    std::cout << "current data time :" << t << std::endl;
    EXPECT_EQ(t.length(), 26);

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


TEST(time_test, days) {
    EXPECT_EQ(time::days(2015), 365);
    EXPECT_EQ(time::days(2016), 366);
    EXPECT_EQ(time::days(2015,1,1), 1);
    EXPECT_EQ(time::days(2015,2,1), 32);
    EXPECT_EQ(time::days(2015,12,31), 365);
    EXPECT_EQ(time::days(2015, 2, 1, 2010, 3, 1), 365 * 5 + 1 + 28);
    std::cout << time::duration_days("2015-12-15") << std::endl;
}

TEST(time_test, performance) {
    uint16_t c = 0;
    for (int i = 0; i < 1000000; ++i) {
//        int a = time::milliseconds();
//        int a = 4 + 5;
        c ++;
//        if (c == 0)
//            std::cout << "== 0" << std::endl;
    }

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