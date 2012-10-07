#ifndef STDLIB_H
#define STDLIB_H

#include <sstream>

#include "NumericUtils.h"
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

  void equals(Environment *env) {
    if (env->expectTwoEqual()) {
      bool result = false;

      switch (env->peekType()) {
      case Number_T:
        {
          double a = env->pop<double>();
          double b = env->pop<double>();
          result = Util::NumericUtils::equalWithEpsilon(a, b);
          break;
        }
      case String_T:
        {
          std::string a = env->pop<std::string>();
          std::string b = env->pop<std::string>();
          result = a.compare(b) == 0;
          break;
        }
      case Boolean_T:
        {
          result = env->pop<bool>() == env->pop<bool>();
          break;
        }
      case Block_T:
        {
          // Compare pointers
          Block *a = env->popBlock();
          Block *b = env->popBlock();
          result = a == b;
          break;
        }
      }

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

  void ifElseCond(Environment *env) {
    if (env->expect(Boolean_T, Block_T, Block_T)) {
      Block *onElse = env->popBlock();
      Block *onIf = env->popBlock();
      bool condition = env->pop<bool>();

      if (condition) {
        env->run(onIf);
      } else {
        env->run(onElse);
      }
    }
  }

  void repeat(Environment *env) {
    if (env->expect(Number_T, Block_T)) {
      Block *b = env->popBlock();
      /**
       * We must bless this block before entering the loop.
       * Otherwise it's values would be deleted after the first
       * iteration (when they are pop'd).
       */
      b->bless();

      int count = (int) env->pop<double>();

      if (count < 0) {
        return;
      } else {
        for (int i = 0; i < count; i++) {
          env->run(b);
        }
      }
    }
  }

  void install(VM &vm) {
    vm.def("dup", dup);
    vm.def("def", def);
    vm.def("=", equals);
    vm.def("if", ifCond);
    vm.def("ifelse", ifElseCond);
    vm.def("repeat", repeat);
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
