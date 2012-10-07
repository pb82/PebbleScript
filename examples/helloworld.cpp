#include "../include/PebbleScript.h"
#include "../include/Stdlib.h"

void hello(PS::Environment *env) {
  env->push("Hello World!");
}

int main () {
  PS::VM vm;
  vm.def("hello", hello);
  PS::Stdlib::install(vm);
  
  vm.eval("hello .");
  return 0;
}
