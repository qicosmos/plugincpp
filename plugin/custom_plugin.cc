//author qicosmos@163.com

#include "../common/plugin.hpp"

namespace purecpp{

    std::string hello(){
      return "hello 2020 pure c++ conference";
    }
    ADD_SERVICE(hello);

    int increment(int i){
      return i + 1;
    }
    ADD_SERVICE(increment);

    int plus(int a, int b){
      return a + b;
    }
    ADD_SERVICE(plus);
}