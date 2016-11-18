//
// Created by liuke on 18/11/2016.
//

#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <algorithm/compress.h>
#include "algo_init.h"

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
                    cmd_factory::instance().callback(param, -1, "operation error");
                }
            } else {
                cmd_factory::instance().callback(param, -1, "parameter error");
            }
        });
    }
}
