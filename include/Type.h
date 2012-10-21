#ifndef TYPE_H
#define TYPE_H

#include <string>

namespace PS {
  /**
   * @brief Possible types of stack items.
   */
  enum DataType {
    Number_T,
    String_T,
    Boolean_T,
    Block_T,
    // The 'Any' type is onle used
    // as a wildcard to query the
    // stack
    Any_T
  };

  /**
   * @brief Type class of a stack item.
   * Each item in the stack has a type and data.
   * The Type class represents the type of a stack item.
   */
  class Type {
  public:
    virtual ~Type() { }
    Type(DataType t) : type(t), blessed(false) { }
    DataType type;

    /**
     * @brief clones this Type-implemeor.
     * @return an exact copy of the Type-implementor.
     */
    virtual Type *clone() const = 0;
    std::string toString();
    std::string toString(DataType t);

    /**
     * @brief prevents the deletion of this item in a pop operation.
     * If this flag is set, the value may not be deleted because
     * it is referenced in the dictionary.
     */
    bool blessed;
  };

  /**
   * @brief return a human-readable string representation of the type
   * @return either "number", "string", "boolean" or "block"
   */
  inline std::string Type::toString() {
    return toString(this->type);
  }

  inline std::string Type::toString(DataType t) {
    switch(t) {
    case Number_T:
      return std::string("number");
    case String_T:
      return std::string("string");
    case Boolean_T:
      return std::string("boolean");
    case Block_T:
      return std::string("block");
    case Any_T:
      return std::string("any");
    default:
      return std::string("<<unknown>>");
    }
  }
}

#endif // TYPE_H
