#ifndef TYPES_H
#define TYPES_H

#include <deque>
#include <string>
#include <cstdlib>

#include "Operation.h"
#include "Value.h"

/**
 * Built-In Types.
 * Number, Stirng, Boolean and Block.
 */

namespace PS {
  /**
   * Numbers are always represented as doubles.
   * Integer arithmetic may be added as a module of integer-operations.
   */
  class Number : public Value<double> {
  public:    
    Number (double v) : Value(v, Number_T) { }
    Number *clone() const {
      return new Number(this->value);
    }
  };

  /**
   * Strings are built into pebble. The string literals are written
   * as 'Hello World!'. Single quotes in strings are possible:
   * 'Hello ''World!''' => Hello 'World!'
   */
  class String : public Value<std::string> {
  public:
    String (std::string v) : Value(v, String_T) { }
    String *clone() const {
      return new String(this->value);
    }
  };

  /**
   * Currently there is a boolean type but no boolean literals.
   * Boolean literals could be implemented in the language with:
   * 'true'   { = 1 1 } def
   * 'false'  { = 1 0 } def
   */
  class Boolean : public Value<bool> {
  public:
    Boolean (bool v) : Value(v, Boolean_T) { }
    Boolean *clone() const {
      return new Boolean(this->value);
    }
  };

  /**
   * Blocks represent a group of operations that are not immediately executed,
   * but pushed on the stack as a single item. They can be assiciated with names
   * in the dictionary which gives them some function-character.
   */
  class Block : public Value<std::deque<Operation> > {
  public:
    Block (std::deque<Operation> v) : Value(v, Block_T) { }
    Block () : Value(std::deque<Operation>(), Block_T) { }
    void bless() {
      this->blessed = true;
      std::deque<Operation>::iterator iter;
      for (iter = this->value.begin(); iter != this->value.end(); iter++) {
        Type *t = (*iter).value;
        if (t->type == Block_T) {
          Block *block = static_cast<Block *>(t);
          block->bless();
        } else {
          t->blessed = true;
        }
      }
    }

    Block *clone() const {
       return new Block(this->value);
    }
  };
}

#endif // TYPES_H
