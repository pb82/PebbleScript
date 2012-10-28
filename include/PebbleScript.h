#ifndef PEBBLESCRIPT_H
#define PEBBLESCRIPT_H

#include "Environment.h"
#include "Fallible.h"
#include "Runnable.h"
#include "Parser.h"
#include "NumericUtils.h"

#include <iostream>

namespace PS {
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
    void call(Type *v);

  private:
    Environment *env;

    /**
     * @brief pointers to (free or static) C++ functions that
     * are associated with names and can be called form inside
     * the script.
     */
    std::map<long, ExternalFunction> externalDefinitions;
  };

  inline VM::~VM() {
    delete env;
  }

  inline VM::VM() : Fallible(), env(new Environment(this, this)) { }
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
    unsigned int index = 0;
    while (index < block->value.size() && !runtimeErrorOccured) {
      Operation op = block->value.at(index++);

      switch(op.opcode) {
      case Push_OC:
        this->env->push(op.value);
        break;
      case Call_OC:
        this->call(op.value);
        break;
      case Minus_OC:
        if (env->expect(Number_T, Number_T)) {
          env->directSub(env->pop<double>());
        }
        break;
      case Plus_OC:
        if (env->expect(Number_T, Number_T)) {
          env->directAdd(env->pop<double>());
        }
        break;
      }
    }

    return !runtimeErrorOccured;
  }

  /**
   * @brief call a function. This can be an externally defined
   * C++ function or a block referenced in the dictionary.
   * @param v the stack item which represents the word to call
   */
  inline void VM::call(Type *v) {
    Number *n = static_cast<Number *>(v);
    long hash = (long) n->value;

    if (externalDefinitions.find(hash) != externalDefinitions.end()) {
      ExternalFunction def = externalDefinitions[hash];
      def(env);
    } else if (env->hasDefinition(hash)) {
      Block *definition = env->getDefinition(hash);
      run(definition);
    } else {
      std::ostringstream ss;
      ss << "Failed to look up the word '";
      ss << n->value;
      ss << "'";
      runtimeError = ss.str();
      raise(ss.str().c_str());
    }
  }
}

#endif // PEBBLESCRIPT_H
