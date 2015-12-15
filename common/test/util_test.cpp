//
// Created by liuke on 15/12/12.
//

#include <test/test_def.h>




#ifdef TEST_ENABLE


#include <util/UUID.h>



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
}

#endif
