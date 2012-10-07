PebbleScript
============

A tiny, header-only scripting engine for C++ applications. PebbleScript implements a little, dynamically typed, stack-based language mostly inspired by PostScript. 
At the moment it is by no means complete.

Hello World
-----------

```  C++
#include "../include/PebbleScript.h"
#include "../include/Stdlib.h"

int main () {
  PS::VM vm;
  PS::Stdlib::install(vm);
  
  vm.eval("'Hello, World!' .");
  return 0;
}
```

Embedding
---------

```  C++
#include "../include/PebbleScript.h"
#include "../include/Stdlib.h"

void hello(PS::Environment *env) {
  env->push("Hello, World!");
}

int main () {
  PS::VM vm;
  vm.def("hello", hello);
  PS::Stdlib::install(vm);
  
  vm.eval("hello .");
  return 0;
}
```

Dependencies
------------

Only the STL for data structures. Nothing else.
