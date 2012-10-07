PebbleScript
============

A tiny, header-only scripting engine for C++ applications. PebbleScript implements a little, dynamically typed, stack-based language mostly inspired by PostScript. 
At the moment it is by no means complete.


Hello World
-----------

```  C++
#include "PebbleScript.h"

void main() {
  PS::VM vm;
  PS::Stdlib::install(vm);

  vm.eval("'Hello, World!' .");
}

```

Embedding
---------

```  C++
#include "PebbleScript.h"

void hello(PS::Environment *env) {
  env->push("Hello, World!");
}

void main() {
  PS::VM vm;
  PS::Stdlib::install(vm);
  
  vm.def("hello", hello);
  vm.eval("hello print");
}

```
