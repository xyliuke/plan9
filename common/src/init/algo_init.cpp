//
// Created by liuke on 18/11/2016.
//

#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <algorithm/compress.h>
#include <algorithm/crypto_wrap.h>
#include "algo_init.h"
#include "error/error_num.h"
namespace plan9
{
    void algo_init::init() {
        log_wrap::io().i("init algo plugin");

        /**
         * 压缩或者解压一个文件或者文件夹
         * action:  (compress/uncompress) 压缩或者解压，必填
         * src_path :  需要操作的文件或者文件夹，如果是压缩，则为文件或者文件夹；如果是解压，则是一个zip包。必填
         * dest_path :  操作之后的目标路径，如果是压缩，则是一个zip包；如果是解压，则为文件或者文件夹；必填
         * code    :   密码，没有可以不填，选填
         */
        cmd_factory::instance().register_cmd("zip", [=](JSONObject param){
            if (param.has("args")) {
                using namespace std;
                JSONObject args = param["args"];
                if (!args.has("action")) {
                    cmd_factory::instance().callback(param, -1, "parameter error, need action parameter");
                    return;
                }
                string action = args.get("action").get_string();

                if (!args.has("src_path")) {
                    cmd_factory::instance().callback(param, -1, "parameter error, need src_path parameter");
                    return;
                }
                string src = args.get("src_path").get_string();

                if (!args.has("dest_path")) {
                    cmd_factory::instance().callback(param, -1, "parameter error, need dest_path parameter");
                    return;
                }
                string dest = args.get("dest_path").get_string();
                bool has_code = args.has("code");
                string code;
                if (has_code) {
                    code = args.get("code").get_string();
                }
                bool ret = false;
                if ("compress" == action) {
                    ret = compress_wrap::compress_zip(src, has_code ? code.c_str() : NULL, dest);
                } else if ("uncompress" == action){
                    ret = compress_wrap::decompress_zip(src, has_code ? code.c_str() : NULL, dest);
                } else {
                    cmd_factory::instance().callback(param, -1, "parameter error, action only support compress/uncompress ");
                    return;
                }
                if (ret) {
                    JSONObject data;
                    data["dest_path"] = dest;
                    cmd_factory::instance().callback(param, ret, data);
                } else {
                    cmd_factory::instance().callback(param, ERROR_OPERATION, ERROR_STRING(ERROR_OPERATION));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
            }
        });

        JSONObject md5_args;
        md5_args.put("text", "hello world", "the string parameter，optional");
        md5_args.put("file", "./a.txt", "the file parameter，optional");
        cmd_factory::instance().register_cmd("md5", [=](JSONObject param){
            if (param.has("args")) {
                JSONObject args = param["args"];
                using namespace std;

                JSONObject result;

                if (args.has("text")) {
                    string text = args["text"].get_string();
                    string ret = crypto_wrap::MD5(text);
                    result["text_md5"] = ret;
                }
                if (args.has("file")) {
                    string file = args["file"].get_string();
                    string ret = crypto_wrap::MD5_file(file);
                    result["file_md5"] = ret;
                }

                cmd_factory::instance().callback(param, true, result);

            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
            }
        });

        JSONObject base64_args;
        base64_args.put("type", "encode/decode", "encode or decode, default encode");
        base64_args.put("text", "hello world", "encode or decode string, required");
        cmd_factory::instance().register_cmd("base64", [=](JSONObject param) mutable {
            if (param.has("args")) {
                JSONObject args = param["args"];
                if (args.has("text")) {
                    bool encode = true;
                    if (args.has("type") && args["type"].get_string() == "decode") {
                        encode = false;
                    }
                    JSONObject ret;
                    if (encode) {
                        ret["data"] = crypto_wrap::base64(args["text"].get_string());
                    } else {
                        ret["data"] = crypto_wrap::base64(args["text"].get_string());
                    }
                    cmd_factory::instance().callback(param, true, ret);
                } else {
                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " , refer to ", base64_args.to_format_string()));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
            }
        });

        JSONObject sha1_args;
        sha1_args.put("text", "hello world", "sha1 input string");
        sha1_args.put("file", "./a.txt", "sha1 input file");
        cmd_factory::instance().register_cmd("sha1", [=](JSONObject param){
            if (param.has("args")) {
                JSONObject args = param["args"];
                JSONObject ret;
                if (args.has("text")) {
                    ret["text_sha1"] = crypto_wrap::SHA1(args["text"].get_string());
                }

                if (args.has("file")) {
                    ret["file_sha1"] = crypto_wrap::SHA1_file(args["file"].get_string());
                }

                cmd_factory::instance().callback(param, true, ret);
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
            }
        });
    }
}
