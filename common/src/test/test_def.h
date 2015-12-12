//
// Created by liuke on 15/12/9.
//

#ifndef COMMON_TEST_DEF_H_H
#define COMMON_TEST_DEF_H_H

#include <gtest/gtest.h>

#define TEST_ENABLE

#ifdef TEST_ENABLE
#define TEST_INIT testing::InitGoogleTest(&argc, argv);
#else
#define TEST_INIT
#endif


#ifdef TEST_ENABLE
#define TEST_RUN RUN_ALL_TESTS()
#else
#define TEST_RUN 0
#endif


#endif //COMMON_TEST_DEF_H_H
