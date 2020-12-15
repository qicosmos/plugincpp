//
// Created by yu.qi on 2020/7/13.
//

#pragma once

#include <msgpack.hpp>

namespace purecpp{

    using buffer_type = msgpack::sbuffer;
    struct msg_codec {
        const static size_t init_size = 2 * 1024;

        template<typename... Args>
        static buffer_type pack_args(Args&&... args) {
            buffer_type buffer(init_size);
            msgpack::pack(buffer, std::forward_as_tuple(std::forward<Args>(args)...));
            return buffer;
        }

        template<typename Arg, typename... Args,
                typename = typename std::enable_if<std::is_enum<Arg>::value>::type>
        static std::string pack_args_str(Arg arg, Args&&... args) {
            buffer_type buffer(init_size);
            msgpack::pack(buffer, std::forward_as_tuple((int)arg, std::forward<Args>(args)...));
            return std::string(buffer.data(), buffer.size());
        }

        template<typename T>
        buffer_type pack(T&& t) const {
            buffer_type buffer;
            msgpack::pack(buffer, std::forward<T>(t));
            return buffer;
        }

        template<typename T>
        T unpack(char const* data, size_t length) {
            try {
                msgpack::unpack(msg_, data, length);
                return msg_.get().as<T>();
            } catch (...) { throw std::invalid_argument("unpack failed: Args not match!"); }
        }

        template<typename Result>
        auto unpack0(char const* data, size_t length) {
            try {
                msgpack::unpack(msg_, data, length);
                return as<Result>();

            } catch (...) { throw std::invalid_argument("unpack failed: arguments not match!"); }
        }

        int error_code(char const* data, size_t length){
          std::tuple<int> code = unpack<std::tuple<int>>(data, length);
          return std::get<0>(code);
        }

        template<typename Result>
        Result result(char const* data, size_t length){
          auto tp = unpack<std::tuple<int, Result>>(data, length);
          return std::get<1>(tp);
        }

    private:
        template<typename R>
        std::enable_if_t<std::is_void<R>::value, std::tuple<int>> as(){
            return msg_.get().as<std::tuple<int>>();
        }

        template<typename R>
        std::enable_if_t<!std::is_void<R>::value, std::tuple<int, R>> as(){
            return msg_.get().as<std::tuple<int, R>>();
        }


        msgpack::unpacked msg_;
    };
}

