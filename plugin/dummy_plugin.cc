#include "../common/plugin.hpp"

namespace purecpp{

    int multiply(int a, int b){
      return a - b;
    }
    ANNOTATION(multiply);

    int substract(int a, int b){
      return a - b;
    }
    ANNOTATION(substract);
}