#ifndef PEBBLESCRIPT_H
#define PEBBLESCRIPT_H

#include "Environment.h"
#include "Fallible.h"
#include "Runnable.h"
#include "Parser.h"
#include "NumericUtils.h"

#include <iostream>

namespace PS {
  struct Continuation {
  public:
    Block *block;
    std::deque<Operation>::iterator iterator;
  };

  /**
   * @brief The virtual machine class. Also the common entry point
   * for using pebble script.
   */
  class VM : public Fallible, public Runnable {
  public:
    VM ();
    ~VM ();

    Environment *eval(const char *source);
    std::string &getError();

    void def(const char *name, ExternalFunction def);

    bool run(Block *block);
    void call(long hash);

  private:
    Environment *env;
    std::stack<Continuation> *continuationStack;

    /**
     * @brief pointers to (free or static) C++ functions that
     * are associated with names and can be called form inside
     * the script.
     */
    std::map<long, ExternalFunction> externalDefinitions;
  };

  inline VM::~VM() {
    delete continuationStack;
    delete env;
  }

  inline VM::VM() : Fallible(), env(new Environment(this, this)), continuationStack(new std::stack<Continuation>) { }
  inline std::string &VM::getError() {
    return this->runtimeError;
  }

  inline void VM::def(const char *name, ExternalFunction def) {
    std::string v = std::string(name);
    long hash = Util::NumericUtils::hash(v);
    externalDefinitions[hash] = def;
  }

  /**
   * @brief evaluate expression.
   * @param source the source code to evaluate.
   * @return a pointer to an Environment object. The stack of the
   * environment can be inspected in order to get the computed
   * values (in case the script wasn't called just for it's side effect).
   *
   * TODO: a better eval function that can return concrete results.
   */
  inline Environment *VM::eval(const char *source) {
    this->runtimeError = std::string("");
    this->runtimeErrorOccured = false;

    Block *block = new Block();
    Parser parser(source, block);

    if (parser.parse()) {
      if (!this->run(block)) {
        return 0;
      } else {
        delete block;
        return this->env;
      }
    } else {
      this->runtimeError = parser.getErrors().front();
      delete block;
      return 0;
    }
  }

  /**
   * @brief execute a block
   * @param block pointer to the block to execute
   */
  inline bool VM::run(Block *block) {
    Continuation c;
    c.block = block;
    c.iterator = block->value.begin();

    continuationStack->push(c);

tc_startover:

    Continuation continuation = continuationStack->top(); continuationStack->pop();
    block = continuation.block;
    std::deque<Operation>::iterator iterator = continuation.iterator;

tc_optimized:

    for (; iterator != block->value.end(); ++iterator) {
      Operation op = *iterator;

      // Seems to be a little faster than a switch statement

      if (op.opcode == Push_OC) {
        this->env->push(op.value);
        continue;
      }

      if (op.opcode == Call_OC) {
        Number *n = static_cast<Number *>(op.value);
        long hash = (long) n->value;

        if (externalDefinitions.find(hash) != externalDefinitions.end()) {

          ExternalFunction def = externalDefinitions[hash];
          def(env);
          continue;
        } else if (env->hasDefinition(hash)) {
          // Tail call?
          if (&*iterator == &block->value.back()) {
            block = env->getDefinition(hash);
            iterator = block->value.begin();
            goto tc_optimized;
          } else {
            Continuation _c;
            _c.block = block;
            _c.iterator = ++iterator;
            continuationStack->push(_c);
            block = env->getDefinition(hash);
            iterator = block->value.begin();
            goto tc_optimized;
          }
        } else {
          std::ostringstream ss;
          ss << "Failed to look up the word '";
          ss << hash;
          ss << "'";
          runtimeError = ss.str();
          raise(ss.str().c_str());
          break;
        }
      }

      if (op.opcode == If_OC) {
        if (env->expect(Boolean_T, Block_T)) {
          Block *b = env->popBlock();
          if (env->pop<bool>()) {
            block = b;
            iterator = block->value.begin();
            goto tc_optimized;
          }
        }
        continue;
      }

      if (op.opcode == Minus_OC) {
        if (env->expect(Number_T, Number_T)) {
          env->directSub(env->pop<double>());
        }
        continue;
      }

      if (op.opcode == Plus_OC) {
        if (env->expect(Number_T, Number_T)) {
          env->directAdd(env->pop<double>());
        }
        continue;
      }

      if (op.opcode == Dup_OC) {
        if (env->expectNotEmpty()) {
          env->directDup();
        }
        continue;
      }

      if (op.opcode == Swap_OC) {
        if (env->expectAtLeast(2)) {
          env->directSwap();
        }
        continue;
      }

      std::ostringstream ss;
      ss << "Unkown Opcode '";
      ss << op.opcode;
      ss << "'";
      runtimeError = ss.str();
      raise(ss.str().c_str());
      break;
    }

    if (continuationStack->size() > 0) {
      goto tc_startover;
    }

    return !runtimeErrorOccured;
  }

  /**
   * @brief call a function. This can be an externally defined
   * C++ function or a block referenced in the dictionary.
   * @param v the stack item which represents the word to call
   */
  inline void VM::call(long hash) {
    if (externalDefinitions.find(hash) != externalDefinitions.end()) {
      ExternalFunction def = externalDefinitions[hash];
      def(env);
    } else if (env->hasDefinition(hash)) {
      Block *definition = env->getDefinition(hash);
      run(definition);
    } else {
      std::ostringstream ss;
      ss << "Failed to look up the word '";
      ss << hash;
      ss << "'";
      runtimeError = ss.str();
      raise(ss.str().c_str());
    }
  }
}

#endif // PEBBLESCRIPT_H
