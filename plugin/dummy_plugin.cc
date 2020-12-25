#include "../common/plugin.hpp"

namespace purecpp{

    int multiply(int a, int b){
      return a * b;
    }
    ADD_SERVICE(multiply);

    int substract(int a, int b){
      return a - b;
    }
    ADD_SERVICE(substract);

    struct dummy_t{
        std::string echo(std::string str){
          return str;
        }

        int add(int a, int b){
          return a + b;
        }
    };

    dummy_t dummy;
    ADD_SERVICE(&dummy_t::echo, &dummy);
    ADD_SERVICE(&dummy_t::add, &dummy);
}