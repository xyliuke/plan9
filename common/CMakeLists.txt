cmake_minimum_required(VERSION 3.3)
project(common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_OSX_ARCHITECTURES  x86_64)

#添加搜索头文件路径
include_directories(../3rd/glog/include ../3rd/jsoncpp/dist ../3rd/boost_1_55_0 ../3rd/googletest/googletest/include)
#添加搜索库路径
link_directories(../3rd/glog/lib/ ../3rd/boost_1_55_0/lib ../3rd/googletest/googletest)

set(SOURCE_FILES main.cpp)

include_directories(header)
include_directories(src)

aux_source_directory(../3rd/jsoncpp/dist JSONCPP_SRC)
aux_source_directory(src/commander COMMADNER_SRC)
aux_source_directory(src/json JSONWRAP_SRC)
aux_source_directory(src/util/ UTIL_SRC)
aux_source_directory(src/log/ LOG_SRC)

aux_source_directory(test/ TEST_SRC)



#生成可执行文件
add_executable(common ${SOURCE_FILES} ${COMMADNER_SRC} ${TEST_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC} ${LOG_SRC})

#生成静态库
#add_library(common ${SOURCE_FILES} ${SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC})


target_link_libraries(common glog boost_system boost_filesystem boost_date_time gtest)