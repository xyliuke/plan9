//
// Created by ke liu on 27/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#ifndef ANET_STATE_MACHINE_H
#define ANET_STATE_MACHINE_H

#include <functional>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <stdlib.h>

namespace plan9 {
    //TODO 实现简单的状态机
    class state_machine;
    class state;


    class state {
    public:
        state();
        virtual void on_entry(std::string event, state_machine* fsm) {}
        virtual void on_exit(std::string event, state_machine* fsm) {}
        virtual const std::type_info& get_type();
        virtual void exec() {}
    };

    class transition_row {
    private:

    public:

        template <typename B, typename E>
        static std::shared_ptr<transition_row> get(state_machine* fsm, std::string event, std::function<bool(state_machine*)> action) {
            std::shared_ptr<transition_row> ret = std::make_shared<transition_row>(event, action);
            ret->set_state<B, E>(fsm);
            return ret;
        }

        /**
         * 定义一个迁移的事件
         * @param event 迁移的事件
         * @param action 迁移的动作，返回true表示同意迁移，返回false表示不同意迁移
         */
        transition_row(const std::string event, std::function<bool(state_machine*)> action)
                : event_(event), action_(action) {
        }
        /**
         * 设置移动的起始和终止状态
         * @tparam B 一条迁移的起始状态类型
         * @tparam E 一条迁移的终止起始状态
         */
        template <typename B, typename E>
        void set_state(state_machine* fsm) {
            b_hash_code = typeid(B).hash_code();
            e_hash_code = typeid(E).hash_code();
            std::shared_ptr<std::map<size_t, std::shared_ptr<state>>> m;
            if (map_.find(fsm) != map_.end()) {
                m = map_[fsm];
            } else {
                m = std::make_shared<std::map<size_t, std::shared_ptr<state>>>();
                map_[fsm] = m;
            }
            if (m->find(b_hash_code) == m->end()) {
                std::shared_ptr<state> obj = std::make_shared<B>();
                (*m)[b_hash_code] = obj;
            }
            if (m->find(e_hash_code) == m->end()) {
                std::shared_ptr<state> obj = std::make_shared<E>();
                (*m)[e_hash_code] = obj;
            }

            set_trace(typeid(B), typeid(E));

        }

        bool is_match(std::string event, size_t begin) {
            return event_ == event && begin == b_hash_code;
        }

        bool exec_action(state_machine* fsm) {
            if (action_) {
                return action_(fsm);
            }
            return true;
        }

        size_t get_end() {
            return e_hash_code;
        }

        static std::shared_ptr<state> get(state_machine* fsm, size_t hash_code) {
            std::shared_ptr<state> ret;
            if (map_.find(fsm) != map_.end()) {
                auto m = map_[fsm];
                if (m->find(hash_code) != m->end()) {
                    return (*m)[hash_code];
                }
            }
            return ret;
        }
        static void remove(state_machine* fsm) {
            map_.erase(fsm);
        }

        std::string to_string() {
            return trace;
        }

    private:
        size_t b_hash_code;
        size_t e_hash_code;
        const std::string event_;
        const std::function<bool(state_machine*)> action_;
        static std::map<state_machine*, std::shared_ptr<std::map<size_t, std::shared_ptr<state>>>> map_;
        std::string trace;
        void set_trace(const std::type_info& t1, const std::type_info& t2);
    };

    class state_machine {
    public:
        state_machine();
        ~state_machine();

        /**
         * 设置初始状态
         * @tparam T 状态的类型
         */
        template <typename T>
        void set_init_state() {
            current = typeid(T).hash_code();
        }
        /**
         * 状态机的开始
         */
        void start();
        /**
         * 触发某个事件
         * @param event 事件名
         */
        void process_event(std::string event);
        /**
         * 找不到对应的迁移状态后触发的函数
         * @param begin 触发前的状态
         * @param event 事件
         */
        virtual void no_transition(std::shared_ptr<state> begin, std::string event);

        template <typename B, typename E>
        void add_row(std::string event, std::function<bool(state_machine*)> action) {
            rows->push_back(transition_row::get<B, E>(this, event, action));
        };
        /**
         * 是否记录迁移动作，默认为false
         * @param trace true表示记录
         */
        void set_trace(bool trace, std::function<void(std::string)> callback);

        /**
         * 判断当前是否指定状态
         * @tparam T 状态类
         * @return true表示当前状态相同
         */
        template <typename T>
        bool is_current_state() {
            const std::type_info& t = typeid(T);
            return t.hash_code() == current;
        }

        std::string get_trace();
    private:
        void record(std::shared_ptr<transition_row> row);
        std::shared_ptr<std::vector<std::shared_ptr<transition_row>>> rows;
        size_t current;
        std::shared_ptr<std::vector<std::shared_ptr<transition_row>>> trace;
        bool is_trace;
        std::function<void(std::string)> trace_callback;
    };

#define STATE_MACHINE_ADD_ROW(state_machine_ptr, begin, event, end, action) (state_machine_ptr)->add_row<begin, end>(event, action)
}


#endif //ANET_STATE_MACHINE_H
