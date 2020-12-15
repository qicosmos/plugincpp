//author qicosmos@163.com

#include <iostream>
#include "common/plugin.hpp"

namespace purecpp{

    std::string hello(){
      return "hello 2020 pure c++ conference";
    }
    ANNOTATION(hello);

    int increment(int i){
      return i + 1;
    }
    ANNOTATION(increment);

    int plus(int a, int b){
      return a + b;
    }
    ANNOTATION(plus);
}