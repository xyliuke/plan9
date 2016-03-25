//
// Created by liuke on 16/3/23.
//

#include <algorithm>
#include "protocol.h"


namespace plan9
{

    //协议起始标志
    static const char PROTOCOL_FIRST_LETTER = (char) 94;
    //协议头部总长度
    static const int PROTOCOL_HEADER_LEN = 9;
    //数据客户端id的下标
    static const int PROTOCOL_ID_INDEX = 1;
    //数据版本的下标
    static const int PROTOCOL_VERSION_INDEX = 5;
    //数据类型的下标
    static const int PROTOCOL_TYPE_INDEX = 6;
    //数据长度的下标
    static const int PROTOCOL_DATA_LEN_INDEX = 7;
    //数据实体的下标
    static const int PROTOCOL_DATA_INDEX = 9;



    static int get_id(const char* data) {
        int a = (unsigned char)data[PROTOCOL_ID_INDEX];
        int b = (unsigned char)data[PROTOCOL_ID_INDEX + 1];
        int c = (unsigned char)data[PROTOCOL_ID_INDEX + 2];
        int d = (unsigned char)data[PROTOCOL_ID_INDEX + 3];

        return (a << 24) + (b << 16) + (c << 8) + d;
    }

    static char get_version(const char* data) {
        return data[PROTOCOL_VERSION_INDEX];
    }

    static char get_type(const char* data) {
        return data[PROTOCOL_TYPE_INDEX];
    }

    static int get_data_len(const char* data) {
        int a = (unsigned char)data[PROTOCOL_DATA_LEN_INDEX];
        int b = (unsigned char)data[PROTOCOL_DATA_LEN_INDEX + 1];
        return (a << 8) + b;
    }

    static std::tuple<char*, int> get_data(const char* data) {
        return std::tuple<char*, int>((char*)(data + PROTOCOL_DATA_INDEX), get_data_len(data));
    }


    std::tuple<char *, int> protocol::create_protocol(int client_id, char version, char server_type, char data_type,
                                                      int len, const char *data) {
        if ((len > 0 && data != nullptr) || len == 0) {
            char* ret = new char[PROTOCOL_HEADER_LEN + len];

            ret[0] = PROTOCOL_FIRST_LETTER;//头部开始标志

            //id
            ret[PROTOCOL_ID_INDEX] = (char)((client_id & 0xFF000000) >> 24);
            ret[PROTOCOL_ID_INDEX + 1] = (char)((client_id & 0x00FF0000) >> 16);
            ret[PROTOCOL_ID_INDEX + 2] = (char)((client_id & 0x0000FF00) >> 8);
            ret[PROTOCOL_ID_INDEX + 3] = (char)(client_id & 0x000000FF);

            //version
            ret[PROTOCOL_VERSION_INDEX] = version;

            //type
            ret[PROTOCOL_TYPE_INDEX] = server_type | data_type;

            //len
            ret[PROTOCOL_DATA_LEN_INDEX] = (char)((len & 0x0000FF00) >> 8);
            ret[PROTOCOL_DATA_LEN_INDEX + 1] = (char)(len & 0x000000FF);

            if (len > 0) {
                //data
                std::copy(data, data + len, ret + PROTOCOL_HEADER_LEN);
            }

            return std::tuple<char*, int >(ret, len + PROTOCOL_HEADER_LEN);
        } else {
            return std::tuple<char*, int >(nullptr, -1);
        }
    }

    std::tuple<char*, int> protocol::create_ping_protocol(int client_id, char version) {
        return create_protocol(client_id, version, CONNECTION_SERVER_TYPE, PING_DATA_TYPE, 0, nullptr);
    }

    std::tuple<bool, int, char, char, char, int, char*> protocol::get_protocol(const char *data, int len) {
        if (data[0] == PROTOCOL_FIRST_LETTER && len >= PROTOCOL_HEADER_LEN) {
            std::tuple<char*, int> d = get_data(data);
            int data_len = std::get<1>(d);
            if (data_len + PROTOCOL_HEADER_LEN <= len) {
                int id = get_id(data);
                char ver = get_version(data);
                char type = get_type(data);
                return std::tuple<bool, int, char, char, char, int, char*>(true, id, ver, type & 0xF0, type & 0x0F, data_len, std::get<0>(d));
            }
        }
        return std::tuple<bool, int, char, char, char, int, char*>(false, 0, 0, 0, 0, 0, nullptr);
    }

    int protocol::remove_first_protocol(char *data, int len) {
        if (data[0] == PROTOCOL_FIRST_LETTER && len >= PROTOCOL_HEADER_LEN) {
            int data_len = get_data_len(data);
            int pro_len = data_len + PROTOCOL_HEADER_LEN;
            if (pro_len <= len) {
                std::copy(data + pro_len, data + len, data);
                return len - pro_len;
            }
        }
        return -1;
    }
}