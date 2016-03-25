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

    EXPECT_EQ(plan9::util::instance().is_suffix("abc\n", '\n'), true);
    EXPECT_EQ(plan9::util::instance().is_suffix("abcd", 'd'), true);
    EXPECT_EQ(plan9::util::instance().is_suffix("abc_", '_'), true);
    EXPECT_EQ(plan9::util::instance().is_suffix("abc\r", '\r'), true);
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

TEST(util_test, random) {
    for (int i = 0; i < 10; ++i) {
        std::cout << plan9::UUID::random() << std::endl;
    }
}

TEST(util_test, char_string) {
    char c[129];
    for (int i = 0; i < 129; ++i) {
        c[i] = (char)i;
    }
//    const char* c = "hello world";
    std::string s = plan9::util::instance().char_to_string(c, 129);
    std::cout << s << std::endl;

    s = plan9::util::instance().char_to_char_string(c, 129);
    std::cout << s << std::endl;

    s = plan9::util::instance().char_to_dex_string(c, 129);
    std::cout << s << std::endl;

    const char c1[] = "我们是祖国的花朵";
    std::string s1 = plan9::util::instance().char_to_string(c1, sizeof(c1));
    std::cout << s1 << std::endl;
}

#endif
