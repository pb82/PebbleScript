#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <vector>
#include <stack>
#include <sstream>
#include <iostream>

#include "Fallible.h"
#include "Runnable.h"
#include "Stack.h"
#include "NumericUtils.h"

namespace PS {  
  class Environment : public Stack {
  public:
    Environment(Fallible *f, Runnable *r);
    ~Environment();

    /**
     * pop operations on the global stack
     */
    template <typename T> T pop();

    Type *popRaw();
    Block *popBlock();

    /**
     * push operations on the global stack
     */
    void push(double v);
    void push(const char *v);
    void push(bool v);
    void push(Type *v);

    /**
     * Peeking
     */
    DataType peekType();

    /**
     * Defining things (functions and constants).
     */
    void def(const char *name, Block *def);
    void def(const char *name, ExternalFunction def);
    bool hasDefinition(long hash);
    Block *getDefinition(long hash);

    void raise(const char *msg);

    /**
     * Running blocks
     */
    bool run(Block *block);

    /**
     * Assertions
     */
    bool expect(DataType a);
    bool expect(DataType a, DataType b);
    bool expect(DataType a, DataType b, DataType c);
    bool expectNotEmpty();
    bool expectAtLeast(unsigned int count);
    bool expectTwoEqual();
    bool expectThreeEqual();

  private:
    Fallible *errorReceiver;
    Runnable *targetMachine;
    std::map<long, Block *> internalDefinitions;
  };

  inline Environment::Environment(Fallible *f, Runnable *r) : errorReceiver(f), targetMachine(r) { }

  /**
   * @brief Environment::~Environment
   * cleanup stack and dictionary.
   */
  inline Environment::~Environment() {
    std::map<long, Block *>::iterator iter;
    for (iter = internalDefinitions.begin(); iter != internalDefinitions.end(); ++iter) {
      Type *t = iter->second;
      delete t;
    }
  }

  /**
   * Pop operations
   */
  template <typename T> inline T Environment::pop() {
     Type *t = Stack::pop();
     Value<T> *v = static_cast<Value<T> *>(t);
     T value = v->value;
     if (!v->blessed) {
       delete v;
     }

     return value;
  }

  inline Block * Environment::popBlock() {
    return (Block *) Stack::pop();
  }

  inline Type *Environment::popRaw() {
    return Stack::pop();
  }

  /**
   * Peeking
   */
  DataType Environment::peekType() {
    Type *v = Stack::top();
    return v->type;
  }

  /**
   * Push operations
   */

  inline void Environment::push(double v) {
    Stack::push(new Number(v));
  }

  inline void Environment::push(const char *v) {
    Stack::push(new String(v));
  }

  inline void Environment::push(bool v) {
    Stack::push(new Boolean(v));
  }

  inline void Environment::push(Type *v) {    
    Stack::push(v);
  }

  inline bool Environment::run(Block *block) {
    return targetMachine->run(block);
  }

  /**
   * @brief def associates blocks with names in the dictionary.
   * def can be used to define funtions or constants.
   * @param name the key for the dictionary
   * @param def the block which is stored with this key in the dictionary.
   */
  inline void Environment::def(const char *name, Block *def) {
    def->bless();
    std::string v = std::string(name);
    long hash = Util::NumericUtils::hash(v);
    internalDefinitions[hash] = def;
  }

  /**
   * @brief def bind native functions
   * @param name the name of the function in pebble
   * @param def a pointer to a C++ function with the signature
   * void (Environment *)
   */
  inline void Environment::def(const char *name, ExternalFunction def) {
    targetMachine->def(name, def);
  }

  inline bool Environment::hasDefinition(long hash) {
    return
        this->internalDefinitions.find(hash) !=
        this->internalDefinitions.end();
  }

  inline Block *Environment::getDefinition(long hash) {
    return this->internalDefinitions[hash];
  }

  inline bool Environment::expect(DataType a) {
    if (!expectAtLeast(1)) {
      return false;
    }

    if(!Stack::expect(a)) {
      Type *t = Stack::top();

      std::ostringstream ss;
      ss << "assertion failed: ";
      ss << "expected (";
      ss << t->toString(a);
      ss << ") but found: (";
      ss << Stack::top()->toString();
      ss << ").";

      raise(ss.str().c_str());
      return false;
    } else {
      return true;
    }
  }

  inline bool Environment::expect(DataType a, DataType b) {
    if (!expectAtLeast(2)) {
      return false;
    }

    if(!Stack::expect(a, b)) {
      Type *t = Stack::top();

      std::ostringstream ss;
      ss << "assertion failed: ";
      ss << "expected (";
      ss << t->toString(b);
      ss << ", ";
      ss << t->toString(a);
      ss << ") but found: (";
      ss << Stack::top()->toString();
      ss << ", ";
      ss << Stack::second()->toString();
      ss << ").";

      raise(ss.str().c_str());
      return false;
    } else {
      return true;
    }
  }

  inline bool Environment::expect(DataType a, DataType b, DataType c) {
    if (!expectAtLeast(3)) {
      return false;
    }

    if(!Stack::expect(a, b, c)) {
      Type *t = Stack::top();

      std::ostringstream ss;
      ss << "assertion failed: ";
      ss << "expected (";
      ss << t->toString(c);
      ss << ", ";
      ss << t->toString(b);
      ss << ", ";
      ss << t->toString(a);
      ss << ") but found: (";
      ss << Stack::top()->toString();
      ss << ", ";
      ss << Stack::second()->toString();
      ss << ", ";
      ss << Stack::third()->toString();
      ss << ").";

      raise(ss.str().c_str());
      return false;
    } else {
      return true;
    }
  }

  inline bool Environment::expectNotEmpty() {
    if (Stack::size() == 0) {
      raise("assertion failed: stack empty");
      return false;
    } else {
      return true;
    }
  }

  inline bool Environment::expectAtLeast(unsigned int count) {
    if (Stack::size() < count) {
      raise("assertion failed: not enough items on stack");
      return false;
    } else {
      return true;
    }
  }

  inline bool Environment::expectTwoEqual() {
    if (!expectAtLeast(2)) {
      return false;
    }

    if (!(Stack::top()->type == Stack::second()->type)) {
      std::ostringstream ss;
      ss << "Expected two equal types but found ";
      ss << Stack::top()->toString();
      ss << " and ";
      ss << Stack::second()->toString();
      raise(ss.str().c_str());
      return false;
    } else {
      return true;
    }
  }

  inline bool Environment::expectThreeEqual() {
    if (!expectAtLeast(3)) {
      return false;
    }

    if(!((Stack::top()->type == Stack::second()->type) &&
         (Stack::second()->type == Stack::third()->type))) {
      std::ostringstream ss;
      ss << "Expected three equal types but found ";
      ss << Stack::top()->toString();
      ss << ", ";
      ss << Stack::second()->toString();
      ss << " and ";
      ss << Stack::third()->toString();
      raise(ss.str().c_str());
      return false;
    } else {
      return true;
    }
  }

  /**
   * @brief raise error
   * @param msg the error message
   */
  inline void Environment::raise(const char *msg) {
    this->errorReceiver->raise(msg);
  }
}

#endif // ENVIRONMENT_H
