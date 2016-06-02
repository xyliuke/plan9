//
// Created by liuke on 16/3/27.
//

#include <algorithm>
#include "compress.h"
#include <zlib.h>
#include <zip.h>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <unzip.h>
#include <util/time.h>


#ifdef WIN32
    #define USEWIN32IOAPI
    #include "iowin32.h"
#endif

namespace plan9
{

    static const int WRITE_BUF_SIZE = 1024 * 1024;

    int compress_wrap::compress(const char *data, int len, char *ret, int* ret_len) {
        unsigned long tmp = (unsigned long)(*ret_len);
        int result = compress2((unsigned char *)ret, &tmp, (unsigned char*)data, len, Z_BEST_COMPRESSION);
        *ret_len = (int)tmp;
        if (result == Z_OK) {
            return *ret_len;
        }
        return -1;
    }

    int compress_wrap::maybe_compressed_size(int len) {
        return (int)compressBound(len);
    }

    int compress_wrap::decompress(const char *data, int len, char *ret, int* ret_len) {
        unsigned long tmp = (unsigned long)(*ret_len);
        int result = uncompress((unsigned char*)ret, &tmp, (unsigned char*)data, len);
        *ret_len = (int)tmp;
        if (result == Z_OK) {
            return *ret_len;
        }
        return -1;
    }

    unsigned long compress_wrap::get_file_crc32(std::string file) {

        if (!boost::filesystem::exists(file) || boost::filesystem::is_directory(file)) {
            return 0;
        }

        unsigned long crc_result = 0;

        std::ifstream ifstream;
        ifstream.open(file, std::ios::binary);

        char buf[WRITE_BUF_SIZE];
        while (!ifstream.eof()) {
            ifstream.read(buf, WRITE_BUF_SIZE);
            std::streamsize size = ifstream.gcount();

            crc_result = crc32(crc_result, (unsigned char*)buf, size);
        }

        ifstream.close();

        return crc_result;
    }

    static void compress_one_file(zipFile zf, std::string need_compress_file, const char* code, std::string out_file) {

        if (out_file.at(0) == '/') {
            out_file.erase(0, 1);
        }

        zip_fileinfo zi;
        int year, month, day, hour, minute, second, microsecond;
        time::current_time(&year, &month, &day, &hour, &minute, &second, &microsecond);
        zi.tmz_date.tm_sec = second;
        zi.tmz_date.tm_min = minute;
        zi.tmz_date.tm_hour = hour;
        zi.tmz_date.tm_mday = day;
        zi.tmz_date.tm_mon = month;
        zi.tmz_date.tm_year = year;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
        zipOpenNewFileInZip3(zf, out_file.c_str(), &zi, NULL, 0, NULL, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION,
                    0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, code, compress_wrap::get_file_crc32(need_compress_file));

        char buffer[WRITE_BUF_SIZE];
        std::ifstream ifstream;
        ifstream.open(need_compress_file, std::ios::binary);
        while (!ifstream.eof()) {
            ifstream.read(buffer, WRITE_BUF_SIZE);
            std::streamsize count = ifstream.gcount();
            zipWriteInFileInZip(zf, buffer, count);
        }

        ifstream.close();

        zipCloseFileInZip(zf);
    }

    static void compress_directory(zipFile zf, std::string need_compress_directory, const char* code, std::string parent_path) {
        namespace f = boost::filesystem;
        f::path p(need_compress_directory);
        if (f::is_directory(p)) {
            std::string parent_path_string = f::system_complete(f::path(parent_path)).string();
            f::recursive_directory_iterator end_it;
            for (f::recursive_directory_iterator it(p); it != end_it ; it ++) {
                f::path cp = f::system_complete(*it);
                if (f::is_regular_file(cp)) {
                    std::string cp_str = cp.string();
                    std::string::size_type pos=0;
                    std::string::size_type a = parent_path_string.size();

                    if ((pos = cp_str.find(parent_path_string, pos)) != std::string::npos) {
                        cp_str.erase(pos, a);
                    }

                    if (cp_str.at(0) == '/') {
                        cp_str.erase(0, 1);
                    }

                    compress_one_file(zf, cp.string(), code, cp_str);
                }
            }
        }
    }


    bool compress_wrap::compress_zip(std::string path, const char *code, std::string out_file) {

        namespace f = boost::filesystem;
        f::path p(out_file);
        f::path op = p.parent_path();
        if (!f::exists(op)) {
            f::create_directories(op);
        }

        zipFile zf;
#ifdef USEWIN32IOAPI
        zlib_filefunc_def ffunc;
        fill_win32_filefunc(&ffunc);
        zf = zipOpen2(filename_try,(opt_overwrite==2) ? 2 : 0,NULL,&ffunc);
#else
        zf = zipOpen(out_file.c_str(), APPEND_STATUS_CREATE);
#endif
        f::path cp(path);
        if (f::exists(cp)) {
            if (f::is_directory(cp)) {
                //压缩目录是文件夹,则压缩这个文件夹下所有文件
                compress_directory(zf, path, code, path);
            } else {
                //只压缩一个文件
                compress_one_file(zf, path, code, cp.filename().string());
            }
        }

        zipClose(zf, "compress by plan9");

        return true;
    }

    static bool extract_one_file(unzFile uf, const char* passwd, std::string out_path) {
        unz_file_info file_info;
        char filename[256];
        int err = unzGetCurrentFileInfo(uf, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0);
        if (err != Z_OK) {
            return false;
        }

        namespace bf = boost::filesystem;
        std::string filename_str(filename);
        if (filename[file_info.size_filename - 1] == '/') {
            //是一个文件夹
            bf::create_directories(bf::path(out_path) / bf::path(filename_str));
        } else {
            //是一个文件
            if (unzOpenCurrentFilePassword(uf, passwd) != Z_OK) {
                unzCloseCurrentFile(uf);
                return false;
            }


            char buf[WRITE_BUF_SIZE];
            int len = 0;

            bf::path full_path = bf::path(out_path) / bf::path(filename_str);
            if (full_path.has_parent_path() && !bf::exists(full_path.parent_path())) {
                bf::create_directories(full_path.parent_path());
            }

            std::ofstream file_stream;
            file_stream.open(full_path.string().c_str(), std::ios::trunc);


            while (true) {
                len = unzReadCurrentFile(uf, buf, WRITE_BUF_SIZE);
                if (len <= 0) {
                    break;
                }
                file_stream.write(buf, len);
            }


            file_stream.close();
            unzCloseCurrentFile(uf);
        }

        return true;
    }

    bool compress_wrap::decompress_zip(std::string file, const char *code, std::string out_path) {

        namespace bf = boost::filesystem;

        if (!bf::exists(file)) {
            return false;
        }

        unzFile uf = NULL;
#ifdef USEWIN32IOAPI
        zlib_filefunc_def ffunc;
        fill_win32_filefunc(&ffunc);
        uf = unzOpen2(zipfilename,&ffunc);
#else
        uf = unzOpen(file.c_str());
#endif

        if (uf == NULL) {
            return false;
        }
        unz_global_info gi;

        if (unzGetGlobalInfo(uf, &gi) != Z_OK) {
            return false;
        }


        bf::path out(out_path);
        if (!bf::exists(out)) {
            bf::create_directories(out);
        } else {
            if (!bf::is_directory(out)) {
                return false;
            }
        }


        for (int i = 0; i < gi.number_entry; ++i) {
            if (!extract_one_file(uf, code, out_path)) {
                return false;
            }

            if ((i + 1) < gi.number_entry) {
                if (unzGoToNextFile(uf) != UNZ_OK) {
                    return false;
                }
            }
        }

        return true;
    }


    void compress_wrap::deal_with_compress(int argc, char **argv) {
        if (argc > 1) {
            std::string type = std::string(argv[1]);
            if (type == "-c") {

                if (argc == 5) {
                    std::string path = std::string(argv[2]);
                    std::string password = std::string(argv[3]);
                    std::string out = std::string(argv[4]);

                    if (compress_zip(path, password.c_str(), out)) {
                        std::cout << "compress files success" << std::endl;
                    } else {
                        std::cout << "compress files failure" << std::endl;
                    }

                } else {
                    std::cout << "parameter illegal, for example : ./common -c ./path password ./a.zip" << std::endl;
                }


            } else if (type == "-d"){
                std::cout << "decompress" << std::endl;

                if (argc == 5) {
                    std::string path = std::string(argv[2]);
                    std::string password = std::string(argv[3]);
                    std::string out = std::string(argv[4]);

                    if (decompress_zip(path, password.c_str(), out)) {
                        std::cout << "decompress files success" << std::endl;
                    } else {
                        std::cout << "decompress files failure" << std::endl;
                    }

                } else {
                    std::cout << "parameter illegal, for example : ./common -d ./a.zip password ./out_path" << std::endl;
                }

            } else {
                std::cout << "parameter illegal, for example: \n compress file use: ./common -c ./path password ./a.zip \n decompress file use : ./common -d ./a.zip password ./out_path" << std::endl;
            }
        }
    }

}