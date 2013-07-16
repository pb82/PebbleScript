PebbleScript
============

A toy, header-only scripting engine for C++ applications. PebbleScript implements a little stack-based language mostly inspired by PostScript.

Features:
  - Very simple C++ Interop
  - Tiny and header only, no linking required
  - Tail call optimization (proper tail calls run in constant memory)

License:
  MIT
  

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


Fibonacci in PebbleScript
-------------------------

```
'fib' {
    dup 1 > 
    { 1 - dup 1 - fib swap fib + }
    if
} def

30 fib . cr
```  


Dependencies
------------

Only the STL. Nothing else.
