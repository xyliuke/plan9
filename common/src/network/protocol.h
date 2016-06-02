//
// Created by liuke on 16/3/23.
//

#ifndef COMMON_PROTOCOL_H
#define COMMON_PROTOCOL_H


#include <tuple>

namespace plan9 {

    class protocol {
    public:

            //前4bit的服务器类型协议
        static const char CONNECTION_SERVER_TYPE = (char) 0x00;
        static const char VERIFY_SERVER_TYPE = (char) 0x10;
        static const char SESSION_SERVER_TYPE = (char) 0x20;
        static const char DATABASE_SERVER_TYPE = (char) 0x30;

        static const char MAJOR_CONFIG_TYPE = (char) 0xE0;
        static const char MINOR_CONFIG_TYPE = (char) 0xF0;


            //后4bit的数据类型协议
        static const char NORMAL_STRING_DATA_TYPE = (char) 0x00;
        static const char NORMAL_JSON_DATA_TYPE = (char) 0x01;
        static const char PING_DATA_TYPE = (char) 0x02;
        static const char PONG_DATA_TYPE = (char) 0x03;
        static const char COMPRESS_JSON_DATA_TYPE = (char) 0x04;
        static const char ENCRYPT_JSON_DATA_TYPE = (char) 0x05;
        static const char ENCRYPT_COMPRESS_JSON_DATA_TYPE = (char) 0x06;
        

        /**
         * 创建一条协议
         * @param client_id 客户端id
         * @param version 客户端版本
         * @param server_type 消息需要服务器处理的类型
         * @param data_type 数据类型
         * @param len 数据实体长度
         * @param data 数据实体
         * @return 返回一个二元组,第一个值为协议报文,第二个是协议报文长度
         */
        static std::tuple<char *, int> create_protocol(int client_id, char version, char server_type,
                                                       char data_type, int len, const char *data);

        /**
         * 创建一条ping包协议
         * @param client_id 客户端id
         * @param version 客户端版本号
         * @return 返回一个二元组,第一个值为协议报文,第二个是协议报文长度
         */
        static std::tuple<char*, int> create_ping_protocol(int client_id, char version);

        /**
         * 解析报文,将数据的第一条报文解析,并分解成不现的数据项
         * @param data 原始数据报文
         * @param len 原始数据报文长度
         * @return 返回分解的数据项,依次为:是否为一条完整报文/客户端id/版本号/服务器类型/数据类型/数据实体的长度/数据实体
         */
        static std::tuple<bool, int, char, char, char, int, char*> get_protocol(const char* data, int len);

        /**
         * 删除数据中的第一条协议
         * @param buf 数据
         * @param len 数据长度
         * @return 返回删除后的数据长度
         */
        static int remove_first_protocol(char* buf, int len);

        static inline bool is_pong_type(std::tuple<bool, int, char, char, char, int, char*> item) {
            if ((bool)std::get<0>(item)) {
                return is_pong_type((char)(std::get<4>(item)));
            }
            return false;
        }

        static inline bool is_pong_type(char type) {
            return PONG_DATA_TYPE == (type & 0x0F);
        }

        static inline bool is_normal_json_type(std::tuple<bool, int, char, char, char, int, char*> item) {
            if ((bool)std::get<0>(item)) {
                return is_normal_json_type((char)(std::get<4>(item)));
            }
            return false;
        }

        static inline bool is_normal_json_type(char type) {
            return NORMAL_JSON_DATA_TYPE == (type & 0x0F);
        }

        static inline bool is_normal_string_type(std::tuple<bool, int, char, char, char, int, char*> item) {
            if ((bool)std::get<0>(item)) {
                return is_normal_string_type((char)(std::get<4>(item)));
            }
            return false;
        }

        static inline bool is_normal_string_type(char type) {
            return NORMAL_STRING_DATA_TYPE == (type & 0x0F);
        }

        static inline bool is_compress_json_type(std::tuple<bool, int, char, char, char, int, char*> item) {
            if ((bool)std::get<0>(item)) {
                return is_compress_json_type((char)(std::get<4>(item)));
            }
            return false;
        }

        static inline bool is_compress_json_type(char type) {
            return COMPRESS_JSON_DATA_TYPE == (type & 0x0F);
        }

        static inline bool is_encrypt_json_type(std::tuple<bool, int, char, char, char, int, char*> item) {
            if ((bool)std::get<0>(item)) {
                return is_encrypt_json_type((char)(std::get<4>(item)));
            }
            return false;
        }

        static inline bool is_encrypt_json_type(char type) {
            return ENCRYPT_JSON_DATA_TYPE == (type & 0x0F);
        }

        static inline bool is_encrypt_compress_json_type(std::tuple<bool, int, char, char, char, int, char*> item) {
            if ((bool)std::get<0>(item)) {
                return is_encrypt_compress_json_type((char)(std::get<4>(item)));
            }
            return false;
        }

        static inline bool is_encrypt_compress_json_type(char type) {
            return ENCRYPT_COMPRESS_JSON_DATA_TYPE == (type & 0x0F);
        }

        static inline char get_protocol_version() {
            return 0x01;
        }



    };

}


#endif //COMMON_PROTOCOL_H
