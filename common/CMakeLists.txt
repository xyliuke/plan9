cmake_minimum_required(VERSION 3.3)
project(common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#添加搜索头文件路径
include_directories(../3rd/googletest/googletest/ ../3rd/googletest/googletest/include ../3rd/glog/include ../3rd/jsoncpp/dist ../3rd/libuv-1.x/dist/include)
#添加搜索库路径
link_directories(../3rd/googletest/googletest/make/ ../3rd/glog/lib/ ../3rd/libuv-1.x/dist/lib)
#添加源文件路径
aux_source_directory(../3rd/jsoncpp/dist JSONCPP_SRC)
aux_source_directory(./commander COMMANDER_SRC)
aux_source_directory(./log LOG_SRC)
aux_source_directory(./thread THREAD_SRC)
aux_source_directory(./network NETWORK_SRC)

set(SOURCE_FILES main.cpp test/test.h)
add_executable(common ${SOURCE_FILES} ${JSONCPP_SRC} ${COMMANDER_SRC} ${LOG_SRC} ${THREAD_SRC} ${NETWORK_SRC})

target_link_libraries(common libgtest.a libglog.a libuv.a)