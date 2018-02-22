cmake_minimum_required(VERSION 3.2)
project(common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SYSDEPMEM "macosx")
set(CMAKE_OSX_ARCHITECTURES  x86_64)

#添加搜索头文件路径
include_directories(../3rd ../3rd/jsoncpp/dist ../3rd/boost_1_63_0 ../3rd/googletest/googletest/include ../3rd/zlib/ ../3rd/unzip ../3rd/curl ../3rd/sqlite
        ../3rd/libuv-1.x/include ../3rd/libuv-1.x/src ../3rd/openssl_header)
#添加搜索库路径
link_directories(../3rd/boost_1_63_0/lib ../3rd/googletest/googletest ../3rd/sqlite ../3rd/libuv-1.x/lib/macosx ../3rd/openssl-1.0.2l)
find_library(COREFOUNDATION CoreFoundation)
find_library(SECURITY Security)
find_library(CFNETWORK CFNetwork)

set(SOURCE_FILES main.cpp)

include_directories(header)
include_directories(src)

aux_source_directory(../3rd/jsoncpp/dist JSONCPP_SRC)
#aux_source_directory(../3rd/7z/C/ LZMA_SRC)
aux_source_directory(../3rd/zlib/ ZLIB_SRC)
aux_source_directory(../3rd/unzip/ ZIP_SRC)
aux_source_directory(../3rd/cryptopp565/ CRYPTO_SRC)
aux_source_directory(../3rd/sqlite/ SQLITE_SRC)
#aux_source_directory(../3rd/freeimage/FreeImage/ FREE_IMG_SRC)
#aux_source_directory(../3rd/freeimage/LibJPEG/ JPEG_SRC)
#aux_source_directory(../3rd/freeimage/LibPNG/ PNG_SRC)
#aux_source_directory(../3rd/freeimage/FreeImageToolkit/ FREE_IMG_TK_SRC)
#aux_source_directory(../3rd/MagickCore/ MAGICK_CORE_SRC)
#aux_source_directory(../3rd/libuv-1-1.x/src UV_SRC)
#aux_source_directory(../3rd/libuv-1-1.x/src/unix UV_UNIX_SRC)
aux_source_directory(src/commander COMMADNER_SRC)
aux_source_directory(src/json JSONWRAP_SRC)
aux_source_directory(src/util/ UTIL_SRC)
aux_source_directory(src/log/ LOG_SRC)
aux_source_directory(src/lua/ LUA_SRC)
aux_source_directory(src/init/ INIT_SRC)
aux_source_directory(src/database/ DATABASE_SRC)
aux_source_directory(src/network/ NETWORK_SRC)
aux_source_directory(src/thread/ THREAD_SRC)
aux_source_directory(src/signal/ SIGNAL_SRC)
aux_source_directory(src/algorithm/ ALG_SRC)
aux_source_directory(src/error/ ERROR_SRC)
aux_source_directory(src/image/ IMG_SRC)
aux_source_directory(src/state/ STATE_SRC)
aux_source_directory(src/ssl/ SSL_SRC)
aux_source_directory(src/proxy/ PROXY_SRC)
aux_source_directory(src/uvwrapper/ UVW_SRC)
aux_source_directory(src/cert/ CERT_SRC)
aux_source_directory(src/common/ COMMON_SRC)


aux_source_directory(test/ TEST_SRC)


#生成可执行文件
add_executable(common ${SOURCE_FILES} ${COMMADNER_SRC} ${TEST_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC}
        ${LOG_SRC} ${LUA_SRC} ${INIT_SRC} ${DATABASE_SRC} ${NETWORK_SRC} ${THREAD_SRC} ${SIGNAL_SRC} ${ALG_SRC}
        ${ZLIB_SRC} ${ZIP_SRC} ${CRYPTO_SRC} ${ERROR_SRC} ${IMG_SRC} ${SQLITE_SRC} ${UV_SRC} ${UV_UNIX_SRC} ${UV_ALL_SRC}
        ${STATE_SRC} ${PROXY_SRC} ${SSL_SRC} ${UVW_SRC} ${CERT_SRC} ${COMMON_SRC}
#        ${FREE_IMG_SRC}
#        ${JPEG_SRC}
#        ${JPEG_SRC} ${PNG_SRC} ${FREE_IMG_TK_SRC}
        )

#生成静态库
#add_library(common ${SOURCE_FILES} ${COMMADNER_SRC} ${TEST_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC} ${LOG_SRC} ${LUA_SRC} ${INIT_SRC} ${DATABASE_SRC} ${NETWORK_SRC} ${THREAD_SRC} ${SIGNAL_SRC} ${ALG_SRC} ${ZLIB_SRC})
#生成动态库
#add_library(common SHARED ${SOURCE_FILES} ${COMMADNER_SRC} ${JSONCPP_SRC} ${JSONWRAP_SRC} ${UTIL_SRC} ${LOG_SRC} ${LUA_SRC} ${INIT_SRC} ${DATABASE_SRC})


target_link_libraries(common boost_system boost_thread boost_filesystem boost_date_time gtest curl uv ssl crypto ${COREFOUNDATION} ${SECURITY}
        ${CFNETWORK})
