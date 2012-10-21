#include "../include/PebbleScript.h"
#include "../include/Stdlib.h"

#include <readline/readline.h>
#include <readline/history.h>

void readLine(PS::VM& vm) {
  char *line = 0;
  line = readline("> ");

  if (line && *line) {
    PS::Environment *env = vm.eval(line);
    add_history(line);
    if (env) {
      std::cout << env->toString() << std::endl;
    } else {
      std::cerr << vm.getError() << std::endl;
    }
  }
}

int main () {
  PS::VM vm;
  PS::Stdlib::install(vm);

  while (true) {
    readLine(vm);
  }
  return 0;
}
