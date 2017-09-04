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
        log_wrap::io().i("register algo plugin");

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

        /**
         *  计算一段字符串或者文件的MD5值
         *  参数：
         *  text    需要计算的字符串
         *  file    需要计算的文件路径，如文件不存在，则返回空字符串
         *  callback:
         *  text_md5    字符串的md5值
         *  file_md5    文件的md5值
         */
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

        /**
         *  base64编码
         *  参数：
         *  type    encode/decode   编码或者解码，默认为编码
         *  text    需要编码或者解码的字符串
         *  callback:
         *  data    编码或者解码后的字符串
         */
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

        /**
         * sha1计算值
         * 参数：
         * text     输入字符串
         * file     输入的文件路径
         * callback:
         * text_sha1    计算字符串后的sha1值
         * file_sha1    计算文件后的sha1值
         */
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

        /**
         * 使用rsa公私钥加密解密，生成一对公私钥功能
         */
        JSONObject rsa;
        rsa.put("type", "encrypt/decrypt/gen", "encrypt or decrypt string used by private key or public key; generate a pair of key");
        rsa.put("text", "encrypt or decrypt content, when type is encrypt or decrypt");
        rsa.put("private_key_file", "the private key file path, when type is encrypt or decrypt");
        rsa.put("public_key_file", "the public key file path, when type is encrypt or decrypt");
        rsa.put("private_key_string", "the private key string, when type is encrypt or decrypt. only use one of private_key_file and private_key_string");
        rsa.put("public_key_string", "the public key string, when type is encrypt or decrypt, only use one of public_key_file and public_key_string");
        rsa.put("bits", "when type is gen, the bits default value is 1024");
        rsa.put("output_public_key_file_path", "if this param is set and type is gen, output type is file, default output type is string");
        rsa.put("output_private_key_file_path", "if this param is set and type is gen, output type is file, default output type is string");
        rsa.put("data/private_key_file", "the result of generate private key file");
        rsa.put("data/public_key_file", "the result of generate public key file");
        rsa.put("data/private_key_string", "the result of generate private key string");
        rsa.put("data/public_key_string", "the result of generate public key string");
        rsa.put("data/encrypt_string", "the result of encrypt by private or public key");
        rsa.put("data/decrypt_string", "the result of decrypt by private or public key");
        log_wrap::io().d("rsa command usage :\n", rsa.to_format_string());
        cmd_factory::instance().register_cmd("rsa", [=](JSONObject param){
            JSONObject args = param["args"];
            if (args.has("type")) {
                std::string type = args["type"].get_string();
                if ("gen" == type) {
                    bool output_string = true;
                    std::string out_pub, out_pri;
                    if (args.has("output_public_key_file_path") && args.has("output_private_key_file_path")) {
                        out_pub = args["output_public_key_file_path"].get_string();
                        out_pri = args["output_private_key_file_path"].get_string();
                        output_string = false;
                    }
                    int bits = 1024;
                    if (args.has("bits")) {
                        int b = args["bits"].get_int();
                        if (b >= 1024 && b % 1024 == 0) {
                            bits = b;
                        }
                    }
                    JSONObject ret;
                    if (output_string) {
                        char private_buf[10240], public_buf[10240];
                        int private_len = 10240, public_len = 10240;
                        bool suc = crypto_wrap::create_rsa_private_public_key((char*)private_buf, &private_len, (char*)public_buf, &public_len, bits);
                        if (suc) {
                            std::string output_private_key(private_buf, private_len);
                            std::string output_public_key(public_buf, public_len);
                            ret["private_key_string"] = output_private_key;
                            ret["public_key_string"] = output_public_key;
                            cmd_factory::instance().callback(param, true, ret);
                        } else {
                            cmd_factory::instance().callback(param, -1, "generate private and public key fail");
                        }
                    } else {
                        bool  suc = crypto_wrap::create_rsa_private_public_key_file(out_pri, out_pub, bits);
                        if (suc) {
                            ret["private_key_file"] = out_pri;
                            ret["public_key_file"] = out_pub;
                        } else {
                            cmd_factory::instance().callback(param, -1, "generate private and public key fail");
                        }
                    }
                } else if ("encrypt" == type) {
                    if (!args.has("text")) {
                        cmd_factory::instance().callback(param, false, "text param must be have when type is encrypt or decrypt");
                        return;
                    }
                    std::string text = args["text"].get_string();
                    std::string ret_string;
                    bool error = false;
                    if (args.has("private_key_file")) {
                        ret_string = crypto_wrap::rsa_private_key_file_encrypt(args["private_key_file"].get_string(), text, &error);
                    } else if (args.has("public_key_file")) {
                        ret_string = crypto_wrap::rsa_public_key_file_encrypt(args["public_key_file"].get_string(), text, &error);
                    } else if (args.has("private_key_string")) {
                        ret_string = crypto_wrap::rsa_private_key_encrypt(args["private_key_string"].get_string(), text, &error);
                    } else if(args.has("public_key_string")) {
                        ret_string = crypto_wrap::rsa_public_key_encrypt(args["public_key_string"].get_string(), text, &error);
                    }
                    if (error) {
                        cmd_factory::instance().callback(param, -1, "encrypt fail");
                    } else {
                        JSONObject ret;
                        ret["encrypt_string"] = ret_string;
                        cmd_factory::instance().callback(param, true, ret);
                    }
                } else if ("decrypt" == type) {
                    if (!args.has("text")) {
                        cmd_factory::instance().callback(param, false, "text param must be have when type is encrypt or decrypt");
                        return;
                    }
                    std::string text = args["text"].get_string();
                    std::string ret_string;
                    bool error = false;
                    if (args.has("private_key_file")) {
                        ret_string = crypto_wrap::rsa_private_key_file_decrypt(args["private_key_file"].get_string(), text, &error);
                    } else if (args.has("public_key_file")) {
                        ret_string = crypto_wrap::rsa_public_key_file_decrypt(args["public_key_file"].get_string(), text, &error);
                    } else if (args.has("private_key_string")) {
                        ret_string = crypto_wrap::rsa_private_key_decrypt(args["private_key_string"].get_string(), text, &error);
                    } else if(args.has("public_key_string")) {
                        ret_string = crypto_wrap::rsa_public_key_decrypt(args["public_key_string"].get_string(), text, &error);
                    }
                    if (error) {
                        cmd_factory::instance().callback(param, -1, "decrypt fail");
                    } else {
                        JSONObject ret;
                        ret["decrypt_string"] = ret_string;
                        cmd_factory::instance().callback(param, true, ret);
                    }

                }
            }
        });
    }
}
