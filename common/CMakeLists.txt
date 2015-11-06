cmake_minimum_required(VERSION 3.3)
project(common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#添加搜索头文件路径
include_directories(../3rd/googletest/googletest/ ../3rd/googletest/googletest/include ../3rd/glog/include ../3rd/jsoncpp/dist)
#添加搜索库路径
link_directories(../3rd/googletest/googletest/make/ ../3rd/glog/lib/)
#添加源文件路径
aux_source_directory(../3rd/jsoncpp/dist JSONCPP_SRC)

set(SOURCE_FILES main.cpp network/tcp_client.cpp network/tcp_client.h test/test.h log/log.cpp log/log.h thread/thread_dispatch.cpp thread/thread_dispatch.h thread/thread_wrap.cpp thread/thread_wrap.h log/log_wrap.cpp log/log_wrap.h json/json_wrap.cpp json/json_wrap.h commander/cmd_factory.cpp commander/cmd_factory.h)
add_executable(common ${SOURCE_FILES} ${JSONCPP_SRC})

target_link_libraries(common libgtest.a libglog.a)