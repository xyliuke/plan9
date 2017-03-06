//
// Created by liuke on 23/02/2017.
//

#include <commander/cmd_factory.h>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include <error/error_num.h>
#include "file_init.h"
#include "log/log_wrap.h"
#include "common.h"

namespace plan9
{
    void file_init::init() {
        log_wrap::io().i("register file plugin");

        //生成一个文件夹
        JSONObject mkdir_param;
        mkdir_param["parent_dir"] = "parent dir (optional), if dir is not absolute path, the default value is data path";
        mkdir_param["dir"] = "absolute path or relative path (required), if relative path, parent_dir is need";
        std::string mkdir_string = mkdir_param.to_format_string();
        log_wrap::io().i("register mkdir function, the parameter list : \n", mkdir_string);
        cmd_factory::instance().register_cmd("mkdir", [=](JSONObject param){
            using namespace std;
            if (param.has("args")) {
                JSONObject args = param["args"];
                if (args.has("dir")) {
                    string dir = args["dir"].get_string();
                    namespace fs = boost::filesystem;
                    if (fs::path(dir).is_absolute()) {
                        if (!fs::exists(dir)) {
                            fs::create_directories(dir);
                        }
                    } else {
                        string parent_dir = common::get_data_path();
                        if (args.has("parent_dir")) {
                            parent_dir = args["parent_dir"].get_string();
                        }
                        if (!fs::exists(parent_dir)) {
                            fs::create_directories(parent_dir);
                        }
                        fs::path p = fs::path(parent_dir) / dir;
                        if (!fs::exists(p)) {
                            fs::create_directories(p);
                        }
                    }
                    cmd_factory::instance().callback(param, true);
                } else {
                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to ", mkdir_string));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER)));
            }
        });

        //复制一个文件或者文件夹到另一个目录，包括文件夹中所有子文件夹和文件
        JSONObject copy_dir_param;
        copy_dir_param["src"] = "file or dir (required)";
        copy_dir_param["dest"] = "dir (required), if the dir not exist, create it";
        copy_dir_param["delete_src"] = "true/false, (optional) delete src file or dir when finish the operation, default is false";
        copy_dir_param["copy_file"] = "true/false, (optional) when src is dir, whether copy file, if false, only create dir in dest, default is true";
        copy_dir_param["new_name"] = "only src is file, the new name after copy file, default is origin file name";
        std::string copy_dir_string = copy_dir_param.to_format_string();
        log_wrap::io().i("register copy_dir function, the parameter list : \n", copy_dir_string);
        cmd_factory::instance().register_cmd("copy", [=](JSONObject param){
            if (param.has("args")) {
                JSONObject args = param["args"];
                if (args.has("src") && args.has("dest")) {
                    using namespace std;
                    string src = args["src"].get_string();
                    string dest = args["dest"].get_string();
                    bool copy_file = true;
                    if (args.has("copy_file")) {
                        copy_file = args["copy_file"].get_bool();
                    }
                    namespace fs = boost::filesystem;
                    if (!fs::exists(dest)) {
                        fs::create_directories(dest);
                    }

                    if (fs::is_directory(src)) {
                        //文件夹
                        util::instance().copy_dir(src, dest, copy_file);
                    } else if (fs::is_regular_file(src)) {
                        //文件
                        fs::path sp(src);
                        fs::path dp(dest);

                        if (args.has("new_name")) {
                            dp = dp / args["new_name"].get_string();
                        } else {
                            dp = dp / sp.filename();
                        }
                        boost::system::error_code code;
                        fs::copy_file(sp, dp, fs::copy_option::overwrite_if_exists, code);
                    } else {
                        cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " src value is not support"));
                        return;
                    }
                    if (args.has("delete_src") && args["delete_src"].get_bool()) {
                        fs::remove_all(src);
                    }

                    cmd_factory::instance().callback(param, true);
                } else {
                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to ", copy_dir_string));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER)));
            }
        });

        JSONObject info_param;
        info_param["dir"] = "dir";
        std::string info_string = info_param.get_string();
        cmd_factory::instance().register_cmd("get_dir_info", [=](JSONObject param){
            if (param.has("args")) {
                JSONObject args = param["args"];
                if (args.has("dir")) {
                    std::string dir = args["dir"].get_string();
                    long cap, free, a;
                    bool ret = util::instance().get_dir_info(dir, &cap, &free, &a);
                    if (ret) {
                        JSONObject result;
                        result["capacity"] = cap;
                        result["free"] = free;
                        result["available"] = a;
                        cmd_factory::instance().callback(param, true, result);
                    } else {
                        cmd_factory::instance().callback(param, ERROR_OPERATION, "operation error");
                    }
                } else {
                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to ", info_string));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to ", info_string));
            }
        });


        JSONObject del_param;
        info_param["file"] = "dir or file";
        std::string del_string = del_param.get_string();
        cmd_factory::instance().register_cmd("del", [=](JSONObject param){
            if (param.has("args")) {
                JSONObject args = param["args"];
                if (args.has("file")) {
                    std::string file = args["file"].get_string();
                    boost::filesystem::remove_all(file);
                } else {
                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to ", del_string));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
            }
        });
    }
}
