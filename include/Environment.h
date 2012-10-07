#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <vector>
#include <stack>
#include <sstream>

#include "Fallible.h"
#include "Runnable.h"
#include "Stack.h"

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
    bool hasDefinition(const char * name);
    Block *getDefinition(const char *name);

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

  private:
    Fallible *errorReceiver;
    Runnable *targetMachine;
    std::map<std::string, Block *> internalDefinitions;
  };

  inline Environment::Environment(Fallible *f, Runnable *r) : errorReceiver(f), targetMachine(r) { }

  /**
   * @brief Environment::~Environment
   * cleanup stack and dictionary.
   */
  inline Environment::~Environment() {
    std::map<std::string, Block *>::iterator iter;
    for (iter = internalDefinitions.begin(); iter != internalDefinitions.end(); ++iter) {
      Type *t = iter->second;
      delete t;
    }
  }

  /**
   * Pop operations
   */
  template <typename T> T Environment::pop() {
     Type *t = Stack::pop();
     Value<T> *v = static_cast<Value<T> *>(t);
     T value = v->value;
     if (!v->blessed) {
       delete v;
     }

     return value;
  }

  Block *Environment::popBlock() {
    Type *t = Stack::pop();
    Block *v = static_cast<Block *>(t);
    return v;
  }

  Type *Environment::popRaw() {
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
    return targetMachine->run(block)        ;
  }

  /**
   * @brief def associates blocks with names in the dictionary.
   * def can be used to define funtions or constants.
   * @param name the key for the dictionary
   * @param def the block which is stored with this key in the dictionary.
   */
  inline void Environment::def(const char *name, Block *def) {
    def->bless();
    internalDefinitions[std::string(name)] = def;
  }

  inline bool Environment::hasDefinition(const char *name) {
    return
        this->internalDefinitions.find(std::string(name)) !=
        this->internalDefinitions.end();
  }

  inline Block *Environment::getDefinition(const char *name) {
    return this->internalDefinitions[std::string(name)];
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
      ss << t->toString(a);
      ss << ", ";
      ss << t->toString(b);
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

    if(!Stack::expect(a, b)) {
      Type *t = Stack::top();

      std::ostringstream ss;
      ss << "assertion failed: ";
      ss << "expected (";
      ss << t->toString(a);
      ss << ", ";
      ss << t->toString(b);
      ss << ", ";
      ss << t->toString(c);
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

  /**
   * @brief raise error
   * @param msg the error message
   */
  inline void Environment::raise(const char *msg) {
    this->errorReceiver->raise(msg);
  }
}

#endif // ENVIRONMENT_H
