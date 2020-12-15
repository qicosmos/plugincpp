#include "../common/plugin.hpp"

namespace purecpp{

    int multiply(int a, int b){
      return a * b;
    }
    ANNOTATION(multiply);

    int substract(int a, int b){
      return a - b;
    }
    ANNOTATION(substract);

    struct dummy_t{
        std::string echo(std::string str){
          return str;
        }

        int add(int a, int b){
          return a + b;
        }
    };

    dummy_t dummy;
    ANNOTATION_MEMBER(dummy_t{}, &dummy_t::echo);
    ANNOTATION_MEMBER(dummy, &dummy_t::add);
}