#include "../../../include/PebbleScript.h"

#include <iostream>

namespace mod_readline {
  void readline(PS::Environment *env) {
    env->push("readline placeholder");
    std::cout << "readline call" << std::endl;
  }

  extern "C" void install(PS::Environment *env) {
    std::cout << "installing module" << std::endl;
    env->def("readline", readline);
    std::cout << "installed module" << std::endl;    
  }
}
