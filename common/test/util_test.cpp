//
// Created by liuke on 15/12/12.
//

#include <test/test_def.h>


#ifdef UTIL_TEST


#include <util/UUID.h>
#include <util/util.h>

TEST(util_test, uuid) {
    std::string uuid1 = plan9::UUID::uuid();
    std::string uuid2 = plan9::UUID::uuid();
    EXPECT_NE(uuid1, uuid2);
    EXPECT_EQ(uuid1.size(), 36);
    EXPECT_EQ(uuid2.size(), 36);
    std::cout << "uuid:" << uuid1 << std::endl;
    std::cout << "uuid:" << uuid2 << std::endl;
}


TEST(util_test, id) {
    std::string id1 = plan9::UUID::id();
    std::string id2 = plan9::UUID::id();
    EXPECT_NE(id1, id2);
    std::cout << "id:" << id1 << std::endl;
    std::cout << "id:" << id2 << std::endl;
//    for (int i = 0; i < 500000; ++i) {
//        std::string id = plan9::UUID::id();
//        std::cout << "id:" << plan9::UUID::id() << std::endl;
//    }

    EXPECT_EQ(plan9::util::instance().isSuffix("abc\n", '\n'), true);
    EXPECT_EQ(plan9::util::instance().isSuffix("abcd", 'd'), true);
    EXPECT_EQ(plan9::util::instance().isSuffix("abc_", '_'), true);
    EXPECT_EQ(plan9::util::instance().isSuffix("abc\r", '\r'), true);
}

TEST(util_test, trim) {
    std::string s = "hello world";
    EXPECT_EQ(s, plan9::util::instance().trim(s));
    std::string s1 = "hello world\n";
    EXPECT_EQ(s, plan9::util::instance().trim(s1));
    std::string s2 = "hello world\t";
    EXPECT_EQ(s, plan9::util::instance().trim(s2));
    std::string s3 = " hello world ";
    EXPECT_EQ(s, plan9::util::instance().trim(s3));
    std::string s4 = "\nhello world";
    EXPECT_EQ(s, plan9::util::instance().trim(s4));
    std::string s5 = "\thello world\r";
    EXPECT_EQ(s, plan9::util::instance().trim(s5));
}

#endif
