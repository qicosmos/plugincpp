//
// Created by yu.qi on 2020/7/13.
//

#pragma once

#include <functional>
#include <boost/dll.hpp>
#include "router.hpp"

namespace purecpp{

    std::string call_in_so(const char *data, std::size_t size) {
      return purecpp::router::get().route(data, size);
    }

    std::string get_key(void* ptr){
      return purecpp::router::get().get_key(ptr);
    }

    template<typename Function>
    int register_handler(std::string const& name, const Function& f){
      purecpp::router::get().register_handler(name, f);

      return 0;
    }

    template<typename Function, typename Self>
    int register_handler(std::string const& name, const Function& f, Self* self) {
      std::string key(name.data()+1, name.size()-1);
      purecpp::router::get().register_handler(key, f, self);

      return 0;
    }

    template<typename Self, typename Function>
    int register_handler(Self&& self, std::string const& name, const Function& f) {
      register_handler(name, f, &self);

      return 0;
    }
}

BOOST_DLL_ALIAS(purecpp::call_in_so, call_in_so);
BOOST_DLL_ALIAS(purecpp::get_key, get_key);

#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#ifdef _MSC_VER // Necessary for edit & continue in MS Visual C++.
# define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
# define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

#define CAT( A, B ) A ## B
#define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define VA_SIZE(...)   VA_NARGS(__VA_ARGS__)

#define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

#define ADD_SERVICE( ... ) VA_SELECT( MY_OVERLOADED, __VA_ARGS__ )

#define MY_OVERLOADED_1( f ) int ANONYMOUS_VARIABLE(var) = purecpp::register_handler(#f, f);
#define MY_OVERLOADED_2( f, t ) int ANONYMOUS_VARIABLE(var) = purecpp::register_handler(#f, f, t);
