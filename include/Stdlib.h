#ifndef STDLIB_H
#define STDLIB_H

#include <sstream>
#include <limits>
#include <cmath>

#include "PebbleScript.h"

namespace PS { namespace Stdlib {

  void dup(Environment *env) {
    if (env->expectNotEmpty()) {
      Type *t = env->popRaw();
      env->push(t);
      env->push(t->clone());
    }
  }

  void plus(Environment *env) {
    if (env->expect(Number_T, Number_T)) {
      double a = env->pop<double>();
      double b = env->pop<double>();
      env->push(a + b);
    }
  }

  void minus(Environment *env) {
    if (env->expect(Number_T, Number_T)) {
      double a = env->pop<double>();
      double b = env->pop<double>();
      env->push(b - a);
    }
  }

  void mul(Environment *env) {
    if (env->expect(Number_T, Number_T)) {
      double a = env->pop<double>();
      double b = env->pop<double>();
      env->push(a * b);
    }
  }

  void div(Environment *env) {
    if (env->expect(Number_T, Number_T)) {
      double a = env->pop<double>();
      double b = env->pop<double>();
      env->push(b / a);
    }
  }

  void print(Environment *env) {
    if (env->expectNotEmpty()) {
      std::ostringstream ss;
      switch(env->peekType()) {
      case String_T:
        ss << env->pop<std::string>();
        break;
      case Number_T:
        ss << env->pop<double>();
        break;
      case Boolean_T:
        ss << env->pop<bool>();
        break;
      case Block_T:
        Block *block = env->popBlock();
        ss << "{Block (";
        ss << block->value.size();
        ss << " items)}";
        break;
      }
      std::cout << ss.str();
    }
  }

  void dump(Environment *env) {
    std::cout << env->toString() << std::endl;
  }

  void cr(Environment *) {
    std::cout << std::endl;
  }

  void def(Environment *env) {
    if (env->expect(String_T, Block_T)) {
      Block *block = env->popBlock();
      std::string name = env->pop<std::string>();
      env->def(name.c_str(), block);
    }
  }

  void numericEquals(Environment *env) {
    if (env->expect(Number_T, Number_T)) {
      double a = env->pop<double>();
      double b = env->pop<double>();

      bool result = std::fabs(a - b) < std::numeric_limits<double>::epsilon();
      env->push(new Boolean(result));
    }
  }

  void ifCond(Environment *env) {
    if (env->expect(Boolean_T, Block_T)) {
      Block *block = env->popBlock();
      if (env->pop<bool>()) {
        env->run(block);
      }
    }
  }

  void install(VM &vm) {
    vm.def("dup", dup);
    vm.def("def", def);
    vm.def("=", numericEquals);
    vm.def("if", ifCond);
    vm.def("+", plus);
    vm.def("-", minus);
    vm.def("*", mul);
    vm.def("/", div);
    vm.def(".", print);
    vm.def("cr", cr);
    vm.def("dump", dump);
  }

} }

#endif // STDLIB_H
