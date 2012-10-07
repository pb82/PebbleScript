#ifndef VALUE_H
#define VALUE_H

#include "Type.h"

namespace PS {
  /**
   * @brief stores data of stack items.
   * Each item in the stack has a type and data.
   * The Value class represents the data part of a stack item.
   */
  template <typename T> class Value : public Type {
  public:
    virtual ~Value() { }
    Value (T v, DataType t) : Type(t), value(v) { }
    T value;
  };
}

#endif // VALUE_H
