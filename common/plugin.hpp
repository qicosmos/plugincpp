//
// Created by yu.qi on 2020/7/13.
//

#pragma once

#include <functional>
#include <boost/dll.hpp>
#include "router.hpp"

static auto& g_router = purecpp::router::get();

namespace purecpp{

    std::string call_in_so(const char *data, std::size_t size) {
      return g_router.route(data, size);
    }

    template<typename Function>
    int register_handler(std::string const& name, Function f){
      g_router.register_handler(name, std::move(f));

      return 0;
    }

    template<typename Function, typename Self>
    int register_handler(std::string const& name, const Function& f, Self* self) {
      g_router.register_handler(name, f, self);

      return 0;
    }

    template<typename Self, typename Function>
    int register_handler(Self&& self, std::string const& name, const Function& f) {
      register_handler(name, f, &self);

      return 0;
    }
}

BOOST_DLL_ALIAS(purecpp::call_in_so, call_in_so);

#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#ifdef _MSC_VER // Necessary for edit & continue in MS Visual C++.
# define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
# define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

#define ANNOTATION(f)\
int ANONYMOUS_VARIABLE(var) = purecpp::register_handler(#f, f);

#define ANNOTATION_MEMBER(t, f)\
int ANONYMOUS_VARIABLE(var) = purecpp::register_handler(t, #f, f);
