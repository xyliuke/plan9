cmake_minimum_required(VERSION 3.3)
project(common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_OSX_ARCHITECTURES  x86_64)

#添加搜索头文件路径
include_directories(../3rd/jsoncpp/dist ../3rd/boost_1_55_0 ../3rd/googletest/googletest/include ../3rd/zlib/ ../3rd/unzip)
#添加搜索库路径
link_directories(../3rd/boost_1_55_0/lib ../3rd/googletest/googletest)

set(SOURCE_FILES main.cpp)

include_directories(header)
include_directories(src)

aux_source_directory(../3rd/jsoncpp/dist JSONCPP_SRC)
#aux_source_directory(../3rd/7z/C/ LZMA_SRC)
aux_source_directory(../3rd/zlib/ ZLIB_SRC)
aux_source_directory(../3rd/unzip/ ZIP_SRC)
aux_source_directory(src/commander COMMADNER_SRC)
aux_source_directory(src/json JSONWRAP_SRC)
aux_source_directory(src/util/ UTIL_SRC)
aux_source_directory(src/log/ LOG_SRC)
aux_source_directory(src/lua/ LUA_SRC)
aux_source_directory(src/init/ INIT_SRC)
aux_source_directory(src/datebase/ DATABASE_SRC)
aux_source_directory(src/network/ NETWORK_SRC)
aux_source_directory(src/thread/ THREAD_SRC)
aux_source_directory(src/signal/ SIGNAL_SRC)
aux_source_directory(src/algorithm/ ALG_SRC)


aux_source_directory(test/ TEST_SRC)



#生成可执行文件
add_executable(common ${SOURCE_FILES} ${COMMADNER_SRC} ${TEST_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC} ${LOG_SRC} ${LUA_SRC} ${INIT_SRC} ${DATABASE_SRC} ${NETWORK_SRC} ${THREAD_SRC} ${SIGNAL_SRC} ${ALG_SRC} ${ZLIB_SRC} ${ZIP_SRC})

#生成静态库
#add_library(common ${SOURCE_FILES} ${COMMADNER_SRC} ${TEST_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC} ${LOG_SRC} ${LUA_SRC} ${INIT_SRC} ${DATABASE_SRC} ${NETWORK_SRC} ${THREAD_SRC} ${SIGNAL_SRC} ${ALG_SRC} ${ZLIB_SRC})
#生成动态库
#add_library(common SHARED ${SOURCE_FILES} ${COMMADNER_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC} ${LOG_SRC} ${LUA_SRC} ${INIT_SRC} ${DATABASE_SRC})


target_link_libraries(common boost_system boost_thread boost_filesystem boost_date_time gtest)