//
// Created by guazi on 9/13/16.
//

#ifndef COMMON_JSONOBJECT_H
#define COMMON_JSONOBJECT_H

#include <memory>
#include <string>
#include <map>
#include <vector>

namespace plan9 {

    class JSONObject {
    public:
        JSONObject();//null类型
        JSONObject(const char*);//解析json字符串
        JSONObject(std::string&);//解析json字符串
        //添加key-value值
        JSONObject(const char* key, int value);
        JSONObject(const char* key, long value);
        JSONObject(const char* key, float value);
        JSONObject(const char* key, double value);
        JSONObject(const char* key, bool value);
        JSONObject(const char* key, const char* value);
        JSONObject(const char* key, std::string& value);
        JSONObject(std::string& key, int value);
        JSONObject(std::string& key, long value);
        JSONObject(std::string& key, float value);
        JSONObject(std::string& key, double value);
        JSONObject(std::string& key, bool value);
        JSONObject(std::string& key, const char* value);
        JSONObject(std::string& key, std::string& value);

        static inline JSONObject createObject() { JSONObject object; object.set_object_type(); return object; }
        static inline JSONObject createArray() { JSONObject object; object.set_array_type(); return object; }

        void parse(std::string& json_string);
        void parse(const char* json_string);
        void parse_file(std::string& file_path);
        void parse_file(const char* file_path);

        /**
         * 设置当前对象为数组类型，通过JSONObject()生成的对象默认为NULL类型
         */
        void set_array_type();
        /**
         * 设置当前对象为对象类型，通过JSONObject()生成的对象默认为NULL类型
         */
        void set_object_type();

        /**
         * 对象是否为bool类型
         * @return
         */
        bool is_bool();
        bool is_int();
        bool is_long();
        bool is_float();
        bool is_double();
        bool is_null();
        bool is_string();
        bool is_array();
        bool is_object();
        bool is_undefined();//没有定义的JSON，一般是通过get返回时，没有找到后的返回值为undefine

        /**
         * 获取该对象中的整型数值
         * @return
         */
        int get_int();
        long get_long();
        float get_float();
        double get_double();
        bool get_bool();
        std::string get_string();
        /**
         * 获取array类型对象的中元素, 如果没有打到该元素，则对象为undefined状态
         * @param index 索引值
         * @return
         */
        JSONObject get(int index);
        /**
         * 获取array类型对象的中元素
         * @param index 索引值
         * @param find 是否有该元素的标志
         * @return
         */
        JSONObject get(int index, bool* find);
        /**
         * 获取object类型对象中的元素, 如果没有打到该元素，则对象为undefined状态
         * @param key 元素key值
         * @return
         */
        JSONObject get(std::string& key);
        JSONObject get(const char* key);
        /**
         * 获取object类型对象中的元素, 如果没有打到该元素，则对象为undefined状态
         * @param key 元素key值
         * @param find 是否存在该key的标志
         * @return
         */
        JSONObject get(std::string& key, bool* find);
        JSONObject get(const char* key, bool* find);
        /**
         * 返回元素的个数，数组和字典会有正确返回值，其他返回为0
         * @return
         */
        unsigned long size();

        /**
         * 向array类型对象中添加元素
         * @param value
         */
        void append(int value);
        void append(long value);
        void append(float value);
        void append(double value);
        void append(bool value);
        void append(const char* value);
        void append(std::string& value);
        void append(JSONObject& value);

        /**
         * 删除array类型中元素
         * @param index 元素的下标
         */
        void remove(int index);
        /**
         * 删除array类型数据中的第一个元素
         */
        void remove_first();
        /**
         * 删除array类型数据中的最后一个元素
         */
        void remove_last();
        /**
         * 删除object类型中的元素
         * @param key 元素key值
         */
        void remove(const char* key);
        void remove(std::string& key);

        /**
         * 添加键值对
         * @param key
         * @param value
         */
        void put(std::string key, int value);
        void put(std::string key, long value);
        void put(std::string key, float value);
        void put(std::string key, double value);
        void put(std::string key, bool value);
        void put(std::string key, const char* value);
        void put(std::string key, std::string& value);
        void put(std::string key, JSONObject& value);

        /**
         * 当前json对象中是否包含key值。支持a.b的操作方式
         * @param key json的键值
         * @return
         */
        bool has(const char* key);
        bool has(std::string& key);

        /**
         * 返回字典中key的集合，如果非object类型的，则返回空的集合
         * @return
         */
        std::shared_ptr<std::vector<std::string>> all_keys();
        /**
         * 返回字典或者数组元素的集合
         * @return
         */
        std::shared_ptr<std::vector<JSONObject>> all_values();
        /**
         * 遍历json对象，对于object类型，only_value为false; 其他类型时only_value为true,key为空字符
         */
        void enumerate(std::function<void(std::string key, JSONObject value, bool only_value)>);
        /**
         * 将json对象转换成字符串
         * @return
         */
        std::string to_string();
        /**
         * 将json对象转换成可视化的字符串
         * @return
         */
        std::string to_format_string();

        //重载运算符
        JSONObject operator[](std::string&);
        JSONObject operator[](const char*);
        JSONObject& operator=(std::string&);
        JSONObject& operator=(const char*);
        JSONObject& operator=(int);
        JSONObject& operator=(long);
        JSONObject& operator=(float);
        JSONObject& operator=(double);
        JSONObject& operator=(bool);
        JSONObject& operator=(JSONObject&);
        operator bool () { return !is_undefined();}

    private:
        bool check_undefined();

    private:
        class JSONObject_impl;
        std::shared_ptr<JSONObject_impl> impl_;
    };

}


#endif //COMMON_JSONOBJECT_H
