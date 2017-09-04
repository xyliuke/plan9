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


#ifdef TEST_ENABLE

//#define COMMON_TEST
//#define IMAGE_TEST
//#define COMMANDER_TEST
//#define JSON_TEST
//#define LOG_TEST
//#define TIME_TEST
#define UTIL_TEST
//#define DATABASE_TEST
//#define LUA_TEST
//#define DATABASE_TEST
//#define NETWORK_TEST
//#define ALGO_TEST
#endif


#endif //COMMON_TEST_DEF_H_H
