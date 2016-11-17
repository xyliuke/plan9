//
// Created by guazi on 9/13/16.
//

#include <iostream>
#include <sstream>
#include <assert.h>
#include <fstream>
#include <curses.h>
#include "JSONObject.h"

namespace plan9 {

    static const char OBJECT_BEGIN = '{';
    static const char OBJECT_END = '}';
    static const char OBJECT_COLON = ':';
    static const char ARRAY_BEGIN = '[';
    static const char ARRAY_END = ']';
    static const char VALUE_SEPARATE = ',';
    static const char STRING_FLAG = '"';
    static const char TRANSFER_FLAG = '\\';

    static std::string double_to_string(double value) {
        std::stringstream ss;
        ss.setf(std::ios::fixed);
        ss.precision(15);
        ss << value;
        std::string ret = ss.str();
        int count = 0;
        for (int i = ret.length() - 1; i >= 0; --i) {
            if (ret.at(i) == '0') {
                count ++;
            } else {
                break;
            }
        }
        if (count == 0) {
            return ret;
        }
        return ret.substr(0, ret.length() - count);
    }

    static std::string chars_to_string(const char* chars, unsigned long begin, unsigned long end) {
        if (begin <= end) {
            char* buf = (char*)malloc(end - begin + 1);

            std::copy(chars + begin, chars + end + 1, buf);
            std::string ret(buf, 0, end - begin + 1);
            free(buf);
            return ret;
        }
        return "";
    }

    static std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    static std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    static std::string& trim(std::string str){
        return ltrim(rtrim(str));
    }

    static void trim(const char* string, unsigned long begin_index, unsigned long end_index, unsigned long *new_begin_index, unsigned long *new_end_index) {
        unsigned long a = begin_index, b = end_index;

        for (unsigned long i = begin_index; i <= end_index; ++i) {
            char c = string[i];
            if (std::isspace(c)) {
                a ++;
            } else {
                break;
            }
        }


        for (unsigned long i = end_index; i >= begin_index; --i) {
            char c = string[i];
            if (std::isspace(c)) {
                b --;
            } else {
                break;
            }
        }

        *new_begin_index = a;
        *new_end_index = b;
    }

    static bool char_is_number(char c) {
        return std::isdigit(c) || '-' == c || '+' == c || 'e' == c || 'E' == c;
    }

    static bool chars_is_number(const char* chars, unsigned long begin, unsigned end, bool* is_double) {
        *is_double = false;
        for (unsigned long i = begin; i <= end; ++i) {
            char c = chars[i];
            if ('.' == c || ('E' == c && '-' == chars[i + 1])) {
                *is_double = true;
                return true;
            }
        }
        return true;
    }

    static long get_long_from_string(const char* string) {
        return std::atol(string);
    }

    static long get_long_from_string(std::string& string) {
        return std::stol(string);
    }

    static double get_double_from_string(const char* string) {
        return std::atof(string);
    }
    static double get_double_from_string(std::string& string) {
        return std::stod(string);
    }

    static char num_buf[100];

    class JSONObject::JSONObject_impl {
    private:
        typedef union {
            int i;
            long l;
            float f;
            double d;
            bool b;
        }_value;

        typedef enum {
            _NULL,
            _INT,
            _LONG,
            _FLOAT,
            _DOUBLE,
            _BOOL,
            _STRING,
            _ARRAY,
            _OBJECT,
            _NUMBER  //内部解析时使用
        }_json_type;

        _json_type type;
        _value value;
        std::shared_ptr<std::map<std::string, std::shared_ptr<JSONObject_impl>>> value_object;
        std::shared_ptr<std::vector<std::shared_ptr<JSONObject_impl>>> value_array;
        std::string value_string;

    public:
        JSONObject_impl() {type = _NULL;}
        JSONObject_impl(int value) {this->value.i = value; this->type = _INT;}
        JSONObject_impl(long value) {this->value.l = value; this->type = _LONG;}
        JSONObject_impl(float value) {this->value.f = value; this->type = _FLOAT;}
        JSONObject_impl(double value) {this->value.d = value; this->type = _DOUBLE;}
        JSONObject_impl(bool value) {this->value.b = value; this->type = _BOOL;}
        JSONObject_impl(const char* value) {
            //TODO 解析JSON格式字符串
            value_string = std::string(value);
            this->type = _STRING;
        }
        JSONObject_impl(std::string value) {
            //TODO 解析JSON格式字符串
            value_string = value;
            this->type = _STRING;
        }
        JSONObject_impl(const char* key, int value) {
            this->value_object.reset(new std::map<std::string, std::shared_ptr<JSONObject_impl>>);
            this->type = _OBJECT;
            std::shared_ptr<JSONObject_impl> json(new JSONObject_impl(value));
            (*(this->value_object))[key] = json;
        }
        JSONObject_impl(const char* key, long value) {
            put(key, value);
        }
        JSONObject_impl(const char* key, float value) {
            put(key, value);
        }
        JSONObject_impl(const char* key, double value) {
            put(key, value);
        }
        JSONObject_impl(const char* key, bool value) {
            put(key, value);
        }
        JSONObject_impl(const char* key, const char* value) {
            put(key, value);
        }
        JSONObject_impl(const char* key, std::string& value) {
            put(key, value);
        }
        JSONObject_impl(const char* key, std::shared_ptr<JSONObject_impl> value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, int value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, long value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, float value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, double value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, bool value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, std::string& value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, const char* value) {
            put(key, value);
        }
        JSONObject_impl(std::string& key, std::shared_ptr<JSONObject_impl> value) {
            put(key, value);
        }
//        ~JSONObject_impl() {
//        }

        std::string get_next_key(const char* json_string, unsigned long begin_index, unsigned long end_index, unsigned long* new_begin_index) {
            unsigned long begin = begin_index;
            bool in_string = false;
            for (unsigned long i = begin_index; i <= end_index; ++i) {
                char c = json_string[i];
                if (in_string) {
                    if (c == STRING_FLAG) {
                        in_string = !in_string;
                    }
                } else {
                    if (c == OBJECT_COLON) {
                        *new_begin_index = i + 1;
                        break;
                    } else if (c == STRING_FLAG) {
                        in_string = !in_string;
                        begin = i + 1;
                    }
                }
            }
            unsigned long ret_begin = begin, ret_end = *new_begin_index - 2;

            unsigned long nn_begin_index, nn_end_index;
            trim(json_string, *new_begin_index, end_index, &nn_begin_index, &nn_end_index);
            *new_begin_index = nn_begin_index;

            trim(json_string, ret_begin, ret_end, &ret_begin, &ret_end);
            //去掉可能的引号
            if (ret_end > ret_begin) {
                if (json_string[ret_begin] == STRING_FLAG) {
                    ret_begin ++;
                }
                if (json_string[ret_end] == STRING_FLAG) {
                    ret_end --;
                }
            }
            if (ret_end >= ret_begin) {
                return chars_to_string(json_string, ret_begin, ret_end);
            }
            return "";
        }

        //value_begin和value_end为获取值的前后索引；new_begin_index和new_end_index为除去value后，下次计算的索引
        void get_next_value(const char* json_string, unsigned long begin_index, unsigned long end_index, unsigned long* value_begin,
                            unsigned long* value_end, unsigned long* new_begin_index,
                                   unsigned long* new_end_index, _json_type* type) {

            *new_begin_index = begin_index;
            *new_end_index = end_index;
            *value_begin = begin_index;
            *value_end = end_index;

            //去掉可能的前冒号
            if (json_string[begin_index] == OBJECT_COLON) {
                *value_begin += 1;
            }

            char first_c = json_string[*value_begin];
            if (char_is_number(first_c)) {
                //数字
                for (unsigned long i = *value_begin + 1; i <= end_index; ++i) {
                    char c = json_string[i];
                    if (c == VALUE_SEPARATE || c == OBJECT_END || c == ARRAY_END) {
                        *value_end = i - 1;
                        *new_begin_index = i + 1;
                        break;
                    }
                }
                *type = _NUMBER;
                if (*new_begin_index > end_index) {
                    *new_begin_index = end_index;
                }

                trim(json_string, *value_begin, *value_end, value_begin, value_end);

            } else if (first_c == '{') {
                //object
                int object_count = 1;
                int array_count = 0;
                bool in_string = false;
                for (unsigned long i = *value_begin + 1; i <= end_index; ++i) {
                    char c = json_string[i];
                    if (object_count == 0 && array_count == 0) {
                        if (c == OBJECT_END || c == ARRAY_END || c == VALUE_SEPARATE) {
                            *new_begin_index = i + 1;
                            *value_end = i - 1;
                            break;
                        } else {
                            if (c == OBJECT_BEGIN) {
                                object_count ++;
                            } else if (c == OBJECT_END) {
                                object_count --;
                            } else if (c == ARRAY_BEGIN) {
                                array_count ++;
                            } else if (c == ARRAY_END) {
                                array_count --;
                            } else if (c == STRING_FLAG) {
                                in_string = !in_string;
                            }
                        }
                    } else {
                        if (in_string) {
                            if (c == STRING_FLAG) {
                                in_string = !in_string;
                            }
                        } else {
                            if (c == OBJECT_BEGIN) {
                                object_count ++;
                            } else if (c == ARRAY_BEGIN) {
                                array_count ++;
                            } else if (c == OBJECT_END) {
                                object_count--;
                                if (i == end_index && object_count == 0 && array_count == 0) {
                                    *new_begin_index = i + 1;
                                    *value_end = i;
                                    break;
                                }
                            } else if (c == ARRAY_END) {
                                array_count -- ;
                                if (i == end_index && object_count == 0 && array_count == 0) {
                                    *new_begin_index = i + 1;
                                    *value_end = i;
                                    break;
                                }
                            } else if (c == STRING_FLAG) {
                                in_string = !in_string;
                            }

                        }
                    }
                }
                *type = _OBJECT;
                if (*new_begin_index > end_index) {
                    *new_begin_index = end_index;
                }
            } else if (first_c == '[') {
                //array
                int object_count = 0;
                int array_count = 1;
                bool in_string = false;
                for (unsigned long i = *value_begin + 1; i <= end_index; ++i) {
                    char c = json_string[i];
                    if (array_count == 0 && object_count == 0) {
                        if (c == VALUE_SEPARATE || c == ARRAY_END || c == OBJECT_END) {
                            *new_begin_index = i + 1;
                            *value_end = i - 1;
                            break;
                        } else {
                            if (c == OBJECT_BEGIN) {
                                object_count ++;
                            } else if (c == ARRAY_BEGIN) {
                                array_count ++;
                            } else if (c == OBJECT_END) {
                                object_count --;
                            } else if ( c == ARRAY_END) {
                                array_count --;
                            } else if (c == STRING_FLAG) {
                                in_string = !in_string;
                            }
                        }
                    } else {
                        if (in_string) {
                            if (c == STRING_FLAG) {
                                in_string = !in_string;
                            }
                        } else {
                            if (c == OBJECT_BEGIN) {
                                object_count ++;
                            } else if (c == ARRAY_BEGIN) {
                                array_count++;
                            } else if (c == OBJECT_END) {
                                object_count --;
                                if (i == end_index && object_count == 0 && array_count == 0) {
                                    *new_begin_index = i + 1;
                                    *value_end = i;
                                    break;
                                }
                            } else if (c == ARRAY_END) {
                                array_count --;
                                if (i == end_index && object_count == 0 && array_count == 0) {
                                    *new_begin_index = i + 1;
                                    *value_end = i;
                                    break;
                                }
                            } else if (c == STRING_FLAG) {
                                in_string = !in_string;
                            }

                        }
                    }
                }
                *type = _ARRAY;
                if (*new_begin_index> end_index) {
                    *new_begin_index = end_index;
                }
            } else if (first_c == '\"') {
                //string
                bool in_string = true;
                for (unsigned long i = *value_begin + 1; i <= end_index; ++i) {
                    char c = json_string[i];
                    if (in_string) {
                        if (c == STRING_FLAG) {
                            in_string = !in_string;

                            if (i == end_index) {
                                *new_begin_index = i + 1;
                                *value_end = i - 1;
                            }
                        }
                    } else {
                        if (c == STRING_FLAG) {
                            in_string = !in_string;
                            if (i == end_index) {
                                *new_begin_index = i + 1;
                                *value_end = i - 1;
                            }
                        } else if (c == VALUE_SEPARATE || c == OBJECT_END || c == ARRAY_END) {
                            *new_begin_index = i + 1;
                            *value_end = i - 1;
                            break;
                        }
                    }
                }

                if (*new_begin_index > end_index) {
                    *new_begin_index = end_index;
                }
                //去掉可能的引号
                if (*value_end > *value_begin) {
                    if (json_string[*value_begin] == STRING_FLAG) {
                        *value_begin += 1;
                    }
                    if (json_string[*value_end] == STRING_FLAG) {
                        *value_end-= 1;
                    }
                }
                *type = _STRING;
            } else {
                if (json_string[*value_begin] == 't' && (end_index - *value_begin >= 3) && json_string[*value_begin + 1] == 'r'
                    && json_string[*value_begin + 2] == 'u' && json_string[*value_begin + 3] == 'e' ) {
                    //true
                    *value_end = *value_begin + 3;
                    for (unsigned long i = *value_end + 1; i <= end_index; ++i) {
                        char c = json_string[i];
                        if (c == VALUE_SEPARATE || c == OBJECT_END || c == ARRAY_END) {
                            *new_begin_index = (i + 1);
                            break;
                        }
                    }
                    *type = _BOOL;
                    if (*new_begin_index > end_index) {
                        *new_begin_index = end_index;
                    }
                } else if (json_string[*value_begin] == 'f' && (end_index - *value_begin >= 4) && json_string[*value_begin + 1] == 'a'
                    && json_string[*value_begin + 2] == 'l' && json_string[*value_begin + 3] == 's' && json_string[*value_begin + 4] == 'e' ) {
                    //true
                    *value_end = *value_begin + 4;
                    for (unsigned long i = *value_end + 1; i <= end_index; ++i) {
                        char c = json_string[i];
                        if (c == VALUE_SEPARATE || c == OBJECT_END || c == ARRAY_END) {
                            *new_begin_index = (i + 1);
                            break;
                        }
                    }
                    *type = _BOOL;
                    if (*new_begin_index> end_index) {
                        *new_begin_index = end_index;
                    }
                } else if (json_string[*value_begin] == 'n' && (end_index - *value_begin >= 3) && json_string[*value_begin + 1] == 'u'
                           && json_string[*value_begin + 2] == 'l' && json_string[*value_begin + 3] == 'l') {
                    //null
                    *value_end = *value_begin + 3;
                    for (unsigned long i = *value_end + 1; i <= end_index; ++i) {
                        char c = json_string[i];
                        if (c == VALUE_SEPARATE || c == OBJECT_END || c == ARRAY_END) {
                            *new_begin_index = (i + 1);
                            break;
                        }
                    }
                    *type = _NULL;
                    if (*new_begin_index > end_index) {
                        *new_end_index = end_index;
                    }
                }
            }
        }

        std::shared_ptr<JSONObject_impl> parse_file(std::string& path) {
            std::stringstream ss;
            std::ifstream ifstream(path, std::ios::in);
            char buf[10240];
            while (!ifstream.eof()) {
                ifstream.read(buf, 10240);
                std::streamsize count = ifstream.gcount();
                ss << std::string(buf, count);
            }
            std::string str = ss.str();
            return parse(str);
        }

        std::shared_ptr<JSONObject_impl> parse(std::string& json_string) {
            return _parse(json_string.c_str(), 0, json_string.length() - 1);
        }


        std::shared_ptr<JSONObject_impl> _parse(const char* json_string, unsigned long b_index, unsigned long e_index) {
            std::shared_ptr<JSONObject_impl> ret(new JSONObject_impl);
            unsigned long new_begin_index, new_end_index;
            trim(json_string, b_index, e_index, &new_begin_index, &new_end_index);

            if (new_end_index > new_begin_index) {
                char c = json_string[new_begin_index];
                if (c == OBJECT_BEGIN) {
                    //object
                    ret->set_object_type();

                    //去掉前后两个{}
                    new_begin_index ++;
                    new_end_index --;

                    trim(json_string, new_begin_index, new_end_index, &new_begin_index, &new_end_index);

                    unsigned long index = 0;
                    _json_type type;

                    unsigned long value_begin, value_end;
                    while (new_end_index > new_begin_index) {
                        std::string key = get_next_key(json_string, new_begin_index, new_end_index, &new_begin_index);

                        get_next_value(json_string, new_begin_index, new_end_index, &value_begin, &value_end, &new_begin_index, &new_end_index, &type);
                        if (type == _OBJECT || type == _ARRAY) {
                            auto sub_value = _parse(json_string, value_begin, value_end);
                            ret->put(key, sub_value);
                        } else if (type == _STRING) {
                            std::string value = chars_to_string(json_string, value_begin, value_end);
                            ret->put(key, value);
                        } else if (type == _NUMBER) {
                            //将字符串解析成数字
                            ret->put(key, parse_number(json_string, value_begin, value_end));
                        } else if (type == _BOOL) {
                            ret->put(key, json_string[value_begin] == 't');
                        } else if (type == _NULL) {
                            ret->put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl()));
                        }
                        if (value_end >= new_end_index) {
                            break;
                        }
                        trim(json_string, new_begin_index, new_end_index, &new_begin_index, &new_end_index);
                    }

                } else if (c == ARRAY_BEGIN) {
                    //array
                    ret->set_array_type((unsigned int)((new_end_index - new_begin_index) / 100));

                    //去掉前后两个[]
                    new_begin_index ++;
                    new_end_index --;

                    trim(json_string, new_begin_index, new_end_index, &new_begin_index, &new_end_index);

                    std::string::size_type index = 0;
                    _json_type type;
                    unsigned long value_begin, value_end;
                    while (new_end_index > new_begin_index) {
                        get_next_value(json_string, new_begin_index, new_end_index, &value_begin, &value_end,
                                       &new_begin_index, &new_end_index, &type);
                        if (type == _OBJECT || type == _ARRAY) {
                            auto sub_value = _parse(json_string, value_begin, value_end);
                            ret->append(sub_value);
                        } else if (type == _STRING) {
                            std::string value = chars_to_string(json_string, value_begin, value_end);
                            ret->append(value);
                        } else if (type == _NUMBER) {
                            //将字符串解析成数字
                            ret->append(parse_number(json_string, value_begin, value_end));
                        } else if (type == _BOOL) {
                            ret->append(json_string[value_begin] == 't');
                        } else if (type == _NULL) {
                            ret->append(std::shared_ptr<JSONObject_impl>(new JSONObject_impl()));
                        }
                        if (value_end >= new_end_index) {
                            break;
                        }
                        trim(json_string, new_begin_index, new_end_index, &new_begin_index, &new_end_index);
                    }
                } else {
                    std::string s(json_string, e_index + 1);
                    ret->set(s);
                }
            }
            return ret;
        }
        std::shared_ptr<JSONObject_impl> parse_number(const char* json_string, unsigned long begin, unsigned long end) {
            std::shared_ptr<JSONObject_impl> ret(new JSONObject_impl);
            bool is_double = true;
            bool is_num = chars_is_number(json_string, begin, end, &is_double);
            if (is_num) {
                std::copy(json_string + begin, json_string + end + 1, num_buf);
                num_buf[end - begin + 1] = '\0';

                if (is_double) {
                    double val = get_double_from_string(num_buf);
                    ret->set(val);
                } else {
                    long val = get_long_from_string(num_buf);
                    if (val > std::numeric_limits<int>::max() || val < std::numeric_limits<int>::min()) {
                        ret->set(val);
                    } else {
                        ret->set(static_cast<int>(val));
                    }
                }
            } else {
                ret->set(0);
            }
            return ret;
        }

        void set_array_type() {
            if (!is_array()) {
                this->value_array.reset(new std::vector<std::shared_ptr<JSONObject_impl>>);
                type = _ARRAY;
            }
        }
        void set_array_type(unsigned int size) {
            if (!is_array()) {
                this->value_array.reset(new std::vector<std::shared_ptr<JSONObject_impl>>);
                this->value_array->reserve(size);
                type = _ARRAY;
            }
        }

        void set_object_type () {
            if (!is_object()) {
                this->value_object.reset(new std::map<std::string, std::shared_ptr<JSONObject_impl>>);
                type = _OBJECT;
            }
        }

        bool is_bool() { return type == _BOOL;}
        bool is_int() { return type == _INT;}
        bool is_long() { return type == _LONG;}
        bool is_float() { return type == _FLOAT;}
        bool is_double() { return type == _DOUBLE;}
        bool is_null() { return type == _NULL;}
        bool is_string() { return type == _STRING;}
        bool is_array() { return type == _ARRAY;}
        bool is_object() { return type == _OBJECT;}
        bool is_number() { return type == _INT || type == _LONG || type == _FLOAT || type == _DOUBLE;}


        int get_int() {
            if (is_number()) {
                if (is_int()) return value.i;
                if (is_long()) return static_cast<int>(value.l);
                if (is_float()) return static_cast<int>(value.f);
                if (is_double()) return static_cast<int>(value.d);
            }
            return 0;
        }

        long get_long() {
            if (is_number()) {
                if (is_int()) return static_cast<long>(value.i);
                if (is_long()) return value.l;
                if (is_float()) return static_cast<long>(value.f);
                if (is_double()) return static_cast<long>(value.d);
            }
            return 0L;
        }

        float get_float() {
            if (is_number()) {
                if (is_int()) return static_cast<float>(value.i);
                if (is_long()) return static_cast<float >(value.l);
                if (is_float()) return value.f;
                if (is_double()) return static_cast<float >(value.d);
            }
            return 0.f;
        }

        double get_double() {
            if (is_number()) {
                if (is_int()) return static_cast<double>(value.i);
                if (is_long()) return static_cast<double>(value.l);
                if (is_float()) return static_cast<double>(value.f);
                if (is_double()) return value.d;
            }
            return 0.f;
        }

        std::string get_string_from_double() {
            return double_to_string(get_double());
        }

        bool get_bool() {
            if (is_bool()) {
                return value.b;
            }
            return false;
        }

        std::string get_string() {
            if (is_string()) {
                return value_string;
            }
            return std::string();
        }

        void append(std::shared_ptr<JSONObject_impl> value) {
            if (is_null()) {
                this->value_array.reset(new std::vector<std::shared_ptr<JSONObject_impl>>);
                set_array_type();
            }
            if (is_array()) {
                if (value) {
                    this->value_array->push_back(value);
                } else {
                    this->value_array->push_back(std::shared_ptr<JSONObject_impl>(new JSONObject_impl));
                }
            }
        }
        void append(int value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }
        void append(long value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }
        void append(float value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }
        void append(double value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }
        void append(bool value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }
        void append(const char* value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }
        void append(std::string& value) {
            std::shared_ptr<JSONObject_impl> v(new JSONObject_impl(value));
            append(v);
        }

        std::shared_ptr<JSONObject_impl> remove(int index) {
            if (is_array() && size() > index) {
                int idx = 0;
                auto it = value_array->begin();
                while (it != value_array->end()) {
                    if (index == idx) {
                        auto ret = *it;
                        value_array->erase(it);
                        return ret;
                    }
                    ++ it;
                    idx ++;
                }
            }
            return std::shared_ptr<JSONObject_impl>();
        }

        std::shared_ptr<JSONObject_impl> remove_last() {
            if (is_array() && size() > 0) {
                auto ret = value_array->at(value_array->size() - 1);
                value_array->pop_back();
                return ret;
            }
            return std::shared_ptr<JSONObject_impl>();
        }

        std::shared_ptr<JSONObject_impl> remove_first() {
            if (is_array() && size() > 0) {
                auto ret = *(value_array->begin());
                value_array->erase(value_array->begin());
                return ret;
            }
            return std::shared_ptr<JSONObject_impl>();
        };

        std::shared_ptr<JSONObject_impl> remove(const char* key) {
            std::string k(key);
            return remove(k);
        }

        std::shared_ptr<JSONObject_impl> remove(std::string& key) {
            if (is_object()) {
                auto it = value_object->find(key);
                if (it != value_object->end()) {
                    auto ret = it->second;
                    value_object->erase(key);
                    return ret;
                }
            }
            return std::shared_ptr<JSONObject_impl>();
        }

        unsigned long size() {
            if (is_array()) {
                return value_array->size();
            } else if (is_object()) {
                return value_object->size();
            }
            return 0UL;
        }

        std::shared_ptr<JSONObject_impl> get(int index) {
            if (is_array() && index >= 0 && index < size()) {
                return value_array->at(index);
            }
            return std::shared_ptr<JSONObject_impl>();
        }


        std::shared_ptr<JSONObject_impl> _get(std::string& key) {
            if (is_object()) {
                auto it = value_object->find(key);
                if (it != value_object->end()) {
                    return it->second;
                }
            }
            return std::shared_ptr<JSONObject_impl>();
        }
        std::shared_ptr<JSONObject_impl> _get_or_create(std::string& key) {
            if (is_object()) {
                auto it = value_object->find(key);
                if (it != value_object->end()) {
                    return it->second;
                } else {
                    std::shared_ptr<JSONObject_impl> value(new JSONObject_impl);
                    put(key, value);
                    return value;
                }
            }
            return std::shared_ptr<JSONObject_impl>(new JSONObject_impl());
        }

        std::shared_ptr<JSONObject_impl> get(std::string& key) {
            if (is_object()) {
                if(key.find(".") != std::string::npos) {
                    //包含.
                    std::shared_ptr<JSONObject_impl> ret;
                    std::string::size_type pos = 0, pre_pos = 0;
                    while ((pos = key.find(".", pos)) != std::string::npos) {
                        std::string str = key.substr(pre_pos, pos - pre_pos);
                        if (pre_pos == 0) {
                            //第一个
                            ret = this->_get(str);
                        } else {
                            ret = ret->get(str);
                        }

                        if (!ret || ret->is_null()) {
                            return ret;
                        }

                        pre_pos = ++ pos;
                    }
                    std::string str = key.substr(pre_pos, key.length() - pre_pos);
                    return ret->get(str);
                } else {
                    return this->_get(key);
                }
            }
            return std::shared_ptr<JSONObject_impl>();
        }
        //存在则返回，不存在则创建
        std::shared_ptr<JSONObject_impl> get_or_create(std::string& key) {
            if (is_object()) {
                std::string::size_type pos = 0, pre_pos = 0;
                pos = key.find(".");
                if (pos != std::string::npos) {
                    //存在.
                    std::shared_ptr<JSONObject_impl> ret;
                    do {
                        std::string str = key.substr(pre_pos, pos - pre_pos);
                        if (pre_pos == 0) {
                            //第一个
                            ret = this->_get_or_create(str);
                        } else {
                            ret->set_object_type();
                            ret = ret->get_or_create(str);
                        }

                        pre_pos = ++ pos;
                    } while ((pos = key.find(".", pos)) != std::string::npos);
                    std::string str = key.substr(pre_pos, key.length() - pre_pos);
                    ret->set_object_type();
                    return ret->get_or_create(str);
                } else {
                    return _get_or_create(key);
                }
            }
            return std::shared_ptr<JSONObject_impl>(new JSONObject_impl);
        }

        std::shared_ptr<JSONObject_impl> get(const char* key) {
            std::string k(key);
            return get(k);
        }

        void put(std::string key, std::shared_ptr<JSONObject_impl> value) {
            if (value.get() == this) {
                assert(false);
                return;
            }
            if (is_null()) {
                this->value_object.reset(new std::map<std::string, std::shared_ptr<JSONObject_impl>>);
                type = _OBJECT;
            }
            if (is_object()) {
                if (value) {
                    (*this->value_object)[key] = value;
                } else {
                    (*this->value_object)[key] = std::shared_ptr<JSONObject_impl>(new JSONObject_impl);
                }
            }
        }

        void put(std::string& key, int value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(std::string& key, long value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(std::string& key, float value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(std::string& key, double value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(std::string& key, bool value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(std::string& key, std::string& value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(std::string& key, const char* value) {
            put(key, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, int value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, long value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, float value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, double value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, bool value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, const char* value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }
        void put(const char* key, std::string& value) {
            std::string k(key);
            put(k, std::shared_ptr<JSONObject_impl>(new JSONObject_impl(value)));
        }

        bool has(const char* key) {
            std::string k(key);
            return has(k);
        }

        bool has(std::string& key) {
            if (is_object()) {
                return get(key) != nullptr;
            }
            return false;
        }

        std::shared_ptr<std::vector<std::string>> all_keys() {
            std::shared_ptr<std::vector<std::string>> ret;
            if (is_object()) {
                ret.reset(new std::vector<std::string>);
                auto it = value_object->begin();
                while (it != value_object->end()) {
                    ret->push_back(it->first);
                    ++ it;
                }
            }
            return ret;
        }

        std::shared_ptr<std::vector<std::shared_ptr<JSONObject_impl>>> all_values() {
            std::shared_ptr<std::vector<std::shared_ptr<JSONObject_impl>>> ret;

            if (is_object()) {
                ret.reset(new std::vector<std::shared_ptr<JSONObject_impl>>);
                auto it = value_object->begin();
                while (it != value_object->end()) {
                    ret->push_back(it->second);
                    ++ it;
                }
            } else if (is_array()) {
                ret.reset(new std::vector<std::shared_ptr<JSONObject_impl>>);
                auto it = value_array->begin();
                while (it != value_array->end()) {
                    ret->push_back(*it);
                    ++ it;
                }
            }
            return ret;
        }
        void enumerate(std::function<void(std::string key, std::shared_ptr<JSONObject_impl> value, bool only_value)> func) {
            if (func) {
                if (is_array()) {
                    auto it = value_array->begin();
                    while (it != value_array->end()) {
                        func("", *it, true);
                        ++ it;
                    }
                } else if (is_object()) {
                    auto it = value_object->begin();
                    while (it != value_object->end()) {
                        func(it->first, it->second, false);
                        ++ it;
                    }
                } else {
                    func("", std::shared_ptr<JSONObject_impl>(this), true);
                }
            }
        }

        void set(const char* value) {
            std::string v(value);
            set(v);
        }

        void set(std::string& value) {
            value_string = value;
            type = _STRING;
        }

        void set(int value) {
            this->value.i = value;
            type = _INT;
        }

        void set(long value) {
            this->value.l = value;
            type = _LONG;
        }

        void set(float value) {
            this->value.f = value;
            type = _FLOAT;
        }

        void set(double value) {
            this->value.d = value;
            type = _DOUBLE;
        }

        void set(bool value) {
            this->value.b = value;
            type = _BOOL;
        }

        void set(std::shared_ptr<JSONObject_impl> value) {
            if (value->is_object()) {
                value_object = value->value_object;
            } else if (value->is_array()) {
                value_array = value->value_array;
            } else if (value->is_string()) {
                value_string = value->value_string;
            } else {
                this->value = value->value;
            }
            type = value->type;
        }

        void add_format_tab(std::stringstream& ss, int tab_num) {
            while (tab_num > 0) {
                ss << "\t";
                tab_num --;
            }
        }

        std::string to_format_string() {
            return to_format_string(1);
        }

        std::string to_format_string(int tab_number) {
            std::stringstream ss;
            if (is_array()) {
                ss << "[\n";
                for (int i = 0; i < value_array->size(); ++i) {
                    add_format_tab(ss, tab_number);
                    ss << (*value_array)[i]->to_format_string(tab_number + 1);
                    if (i != (value_array->size() - 1)) {
                        ss << ",";
                    }
                    ss << "\n";
                }
                add_format_tab(ss, tab_number - 1);
                ss << "]";
            } else if (is_object()) {
                ss << "{\n";
                unsigned long size = value_object->size();
                unsigned long index = 0;
                auto it = value_object->begin();
                while (it != value_object->end()) {
                    add_format_tab(ss, tab_number);
                    ss << it->first << ":" << it->second->to_format_string(tab_number + 1);

                    if (index != (size - 1)) {
                        ss << ",";
                    }
                    ss << "\n";
                    ++ index;
                    ++ it;
                }
                add_format_tab(ss, tab_number - 1);
                ss << "}";
            } else {
                if (is_int()) {
                    ss << get_int();
                } else if (is_double()) {
                    ss << get_string_from_double();
                } else if (is_string()) {
                    ss << "\"" << get_string() << "\"";
                } else if (is_float()) {
                    ss << get_float();
                } else if (is_long()) {
                    ss << get_long();
                } else if (is_bool()) {
                    if (get_bool()) {
                        ss << "true";
                    } else {
                        ss << "false";
                    }
                } else if (is_null()) {
                    ss << "null";
                }
            }
            return ss.str();
        }

        std::string to_string() {
            std::stringstream ss;
            if (is_array()) {
                ss << "[";
                for (int i = 0; i < value_array->size(); ++i) {
                    ss << (*value_array)[i]->to_string();
                    if (i != (value_array->size() - 1)) {
                        ss << ",";
                    }
                }
                ss << "]";
            } else if (is_object()) {
                ss << "{";
                unsigned long size = value_object->size();
                unsigned long index = 0;
                auto it = value_object->begin();
                while (it != value_object->end()) {

                    ss << it->first << ":" << it->second->to_string();

                    if (index != (size - 1)) {
                        ss << ",";
                    }
                    ++ index;
                    ++ it;
                }
                ss << "}";
            } else {
                if (is_int()) {
                    ss << get_int();
                } else if (is_double()) {
                    ss << get_string_from_double();
                } else if (is_string()) {
                    ss << "\"" << get_string() << "\"";
                } else if (is_float()) {
                    ss << get_float();
                } else if (is_long()) {
                    ss << get_long();
                } else if (is_bool()) {
                    if (get_bool()) {
                        ss << "true";
                    } else {
                        ss << "false";
                    }
                } else if (is_null()) {
                    ss << "null";
                }
            }
            return ss.str();
        }
    };

    JSONObject::JSONObject() : impl_(new JSONObject_impl){

    }
    JSONObject::JSONObject(const char* value) : impl_(new JSONObject_impl(value)) {
        std::string v(value);
        impl_->set(impl_->parse(v));
    }
    JSONObject::JSONObject(std::string & value) : impl_(new JSONObject_impl(value)) {
        impl_->set(impl_->parse(value));
    }
    JSONObject::JSONObject(const char *key, int value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(const char *key, long value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(const char *key, float value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(const char *key, double value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(const char *key, bool value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(const char *key, const char* value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(const char *key, std::string& value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, int value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, long value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, float value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, double value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, bool value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, const char* value) : impl_(new JSONObject_impl(key, value)){

    }
    JSONObject::JSONObject(std::string& key, std::string& value) : impl_(new JSONObject_impl(key, value)){

    }

    JSONObject::JSONObject(std::shared_ptr<JSONObject_impl> value) : impl_(value) {

    }

    void JSONObject::parse(const char *json_string) {
        std::string string(json_string);
        parse(string);
    }

    void JSONObject::parse(std::string &json_string) {
        if (check_undefined()) return;
        impl_->set(impl_->parse(json_string));
    }

    void JSONObject::parse_file(const char *file_path) {
        std::string file(file_path);
        parse_file(file);
    }

    void JSONObject::parse_file(std::string &file_path) {
        if (check_undefined()) return;
        impl_->set(impl_->parse_file(file_path));
    }

    bool JSONObject::check_undefined() {
        bool is = is_undefined();
        if (is) {
            fprintf(stderr, "error, the json object is undefined\n");
        }
        return is;
    }

    void JSONObject::set_array_type() {
        if (check_undefined()) return;
        impl_->set_array_type();
    }
    void JSONObject::set_object_type() {
        if (check_undefined()) return;
        impl_->set_object_type();
    }

    bool JSONObject::is_bool() {if (check_undefined()) return false; return impl_->is_bool();}
    bool JSONObject::is_int() {if (check_undefined()) return false; return impl_->is_int();}
    bool JSONObject::is_long() {if (check_undefined()) return false; return impl_->is_long();}
    bool JSONObject::is_float() {if (check_undefined()) return false; return impl_->is_float();}
    bool JSONObject::is_double() {if (check_undefined()) return false; return impl_->is_double();}
    bool JSONObject::is_null() {if (check_undefined()) return false; return impl_->is_null();}
    bool JSONObject::is_string() {if (check_undefined()) return false; return impl_->is_string();}
    bool JSONObject::is_array() {if (check_undefined()) return false; return impl_->is_array();}
    bool JSONObject::is_object() {if (check_undefined()) return false; return impl_->is_object();}
    bool JSONObject::is_undefined() { return impl_ == nullptr; }

    int JSONObject::get_int() {
        if (check_undefined()) return 0;
        return impl_->get_int();
    }

    long JSONObject::get_long() {
        if (check_undefined()) return 0;
        return impl_->get_long();
    }

    float JSONObject::get_float() {
        if (check_undefined()) return 0;
        return impl_->get_float();
    }

    double JSONObject::get_double() {
        if (check_undefined()) return 0;
        return impl_->get_double();
    }

    bool JSONObject::get_bool() {
        if (check_undefined()) return false;
        return impl_->get_bool();
    }

    std::string JSONObject::get_string() {
        if (check_undefined()) return "undefined";
        return impl_->get_string();
    }

    unsigned long JSONObject::size() {
        if (check_undefined()) return 0;
        return impl_->size();
    }

    void JSONObject::append(int value) {
        if (check_undefined()) return;
        impl_->append(value);
    }

    void JSONObject::append(long value) {
        if (check_undefined()) return;
        impl_->append(value);
    }

    void JSONObject::append(float value) {
        if (check_undefined()) return;
        impl_->append(value);
    }

    void JSONObject::append(double value) {
        if (check_undefined()) return;
        impl_->append(value);
    }

    void JSONObject::append(bool value) {
        if (check_undefined()) return;
        impl_->append(value);
    }
    void JSONObject::append(const char* value) {
        if (check_undefined()) return;
        impl_->append(value);
    }
    void JSONObject::append(std::string value) {
        if (check_undefined()) return;
        impl_->append(value);
    }
    void JSONObject::append(JSONObject value) {
        if (check_undefined()) return;
        impl_->append(value.impl_);
    }

    JSONObject JSONObject::remove(int index) {
        if (check_undefined()) return JSONObject();
        JSONObject ret(impl_->remove(index));
        return ret;
    }

    JSONObject JSONObject::remove_first() {
        if (check_undefined()) return JSONObject();
        return JSONObject(impl_->remove_first());
    }

    JSONObject JSONObject::remove_last() {
        if (check_undefined()) return JSONObject();
        return JSONObject(impl_->remove_last());
    }

    JSONObject JSONObject::remove(const char *key) {
        if (check_undefined()) return JSONObject();
        return JSONObject(impl_->remove(key));
    }

    JSONObject JSONObject::remove(std::string key) {
        if (check_undefined()) return JSONObject();
        return JSONObject(impl_->remove(key));
    }

    JSONObject JSONObject::get(int index) {
        JSONObject object;
        auto value = impl_->get(index);
        if (value) {
            object.impl_ = value;
        }
        return object;
    }

    JSONObject JSONObject::get(int index, bool *find) {
        JSONObject ret = get(index);
        *find = !(ret.is_undefined());
        return ret;
    }


    JSONObject JSONObject::get(std::string key) {
        JSONObject object;
        std::shared_ptr<JSONObject_impl> im = impl_->get(key);
        object.impl_ = im;
        return object;
    }

    JSONObject JSONObject::get(const char *key) {
        std::string s(key);
        return get(s);
    }

    JSONObject JSONObject::get(const char *key, bool *find) {
        JSONObject ret = get(key);
        *find = !(ret.is_undefined());
        return ret;
    }
    JSONObject JSONObject::get(std::string key, bool *find) {
        JSONObject ret = get(key);
        *find = !(ret.is_undefined());
        return ret;
    }

    std::string JSONObject::to_string() {
        if (check_undefined()) return "undefined";
        return impl_->to_string();
    }

    std::string JSONObject::to_format_string() {
        if (check_undefined()) return "undefined";
        return impl_->to_format_string();
    }

    void JSONObject::put(std::string key, const char *value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    void JSONObject::put(std::string key, std::string value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    void JSONObject::put(std::string key, JSONObject value) {
        if (check_undefined()) return;
        impl_->put(key, value.impl_);
    }

    void JSONObject::put(std::string key, int value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    void JSONObject::put(std::string key, float value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    void JSONObject::put(std::string key, double value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    void JSONObject::put(std::string key, long value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    void JSONObject::put(std::string key, bool value) {
        if (check_undefined()) return;
        std::shared_ptr<JSONObject_impl> object(new JSONObject_impl(value));
        impl_->put(key, object);
    }

    bool JSONObject::has(const char *key) {
        if (check_undefined()) return false;
        return impl_->has(key);
    }

    bool JSONObject::has(std::string key) {
        if (check_undefined()) return false;
        return impl_->has(key);
    }

    std::shared_ptr<std::vector<std::string>> JSONObject::all_keys() {
        if (check_undefined()) return std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>);
        return impl_->all_keys();
    }

    std::shared_ptr<std::vector<JSONObject>> JSONObject::all_values() {
        if (check_undefined()) return std::shared_ptr<std::vector<JSONObject>>(new std::vector<JSONObject>);
        auto values = impl_->all_values();
        auto it = values->begin();
        std::shared_ptr<std::vector<JSONObject>> ret(new std::vector<JSONObject>);
        while (it != values->end()) {
            JSONObject object;
            object.impl_ = *it;
            ret->push_back(object);
            ++ it;
        }
        return ret;
    }

    void JSONObject::enumerate(std::function<void(std::string key, JSONObject value, bool only_value)> func) {
        if (func) {
            if (check_undefined()) return;
            impl_->enumerate([=](std::string key, std::shared_ptr<JSONObject_impl> value, bool only_value){
                JSONObject object;
                object.impl_ = value;
                func(key, object, only_value);
            });
        }
    }

    JSONObject JSONObject::operator[](std::string &key) {
        JSONObject ret;
        if (check_undefined()) return ret;
        impl_->set_object_type();
        auto im = impl_->get_or_create(key);
        ret.impl_ = im;
        return ret;
    }

    JSONObject JSONObject::operator[](const char *key) {
        JSONObject ret;
        if (check_undefined()) return ret;
        impl_->set_object_type();
        std::string k(key);
        auto im = impl_->get_or_create(k);
        ret.impl_ = im;
        return ret;
    }

    JSONObject JSONObject::operator=(JSONObject value) {
        this->impl_->set(value.impl_);
        return *this;
    }

    JSONObject &JSONObject::operator=(std::string value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }

    JSONObject &JSONObject::operator=(const char *value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }

    JSONObject &JSONObject::operator=(int value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }

    JSONObject &JSONObject::operator=(long value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }

    JSONObject &JSONObject::operator=(float value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }

    JSONObject &JSONObject::operator=(double value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }

    JSONObject &JSONObject::operator=(bool value) {
        if (!check_undefined()) {
            impl_->set(value);
        }
        return *this;
    }



}
