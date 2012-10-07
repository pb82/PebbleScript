#ifndef STACK_H
#define STACK_H

#include "Types.h"

namespace PS {
  /**
   * @brief Custom stack implementation on top of std::deque.
   * Has additional features to probe it's elements.
   */

  class Stack {
  public:
    Stack() { }
    ~Stack();

    std::string toString();
  protected:
    Type *pop();
    /**
     * @brief top, second and third probe the corresponding items on the stack
     * without removing them.
     * @return a pointer to the corresponding stack item.
     */
    Type *top();
    Type* second();
    Type* third();
    void push(Type *v);
    bool empty();
    unsigned int size();

    /**
     * @brief expect assert a certain stack condition
     * @param a b c -> First(c) Second(b) Third(a)
     * @return a bool indicating if the expected condition was met
     */
    bool expect(DataType a);
    bool expect(DataType a, DataType b);
    bool expect(DataType a, DataType b, DataType c);

  private:
    std::deque<Type *> data;
  };

  inline Stack::~Stack() {
    std::deque<Type *>::iterator iter;
    for (iter = data.begin(); iter != data.end(); ++iter) {
      delete *iter;
    }
  }

  inline Type *Stack::pop() {
    Type *v = data.front(); data.pop_front();
    return v;
  }

  inline Type *Stack::top() {
    return data.at(0);
  }

  inline Type *Stack::second() {
    return data.at(1);
  }

  inline Type *Stack::third() {
    return data.at(2);
  }

  inline void Stack::push(Type *v) {
    data.push_front(v);
  }

  inline bool Stack::empty() {
    return this->data.empty();
  }

  inline unsigned int Stack::size() {
    return this->data.size();
  }

  inline bool Stack::expect(DataType a) {
    return data.at(0)->type == a;
  }

  inline bool Stack::expect(DataType a, DataType b) {
    return
        data.at(1)->type == a &&
        data.at(0)->type == b;
  }

  inline bool Stack::expect(DataType a, DataType b, DataType c) {
    return
        data.at(2)->type == a &&
        data.at(1)->type == b &&
        data.at(0)->type == c;
  }

  inline std::string Stack::toString() {
    std::deque<Type *>::iterator iter;
    std::ostringstream ss;
    bool isFirst = true;
    ss << "(top) <- ";

    for (iter = data.begin(); iter != data.end(); ++iter) {
      if (!isFirst) {
        ss << ", ";
      }

      Type *t = *iter;
      switch(t->type) {
      case String_T:
        {
        String *s = static_cast<String *>(t);
        ss << s->value;
        break;
        }
      case Number_T:
        {
        Number *s = static_cast<Number *>(t);
        ss << s->value;
        break;
        }
      case Boolean_T:
        {
        Boolean *s = static_cast<Boolean *>(t);
        ss << (s->value ? "true" : "false");
        break;
        }
      case Block_T:
        {
        Block *s = static_cast<Block *>(t);
        ss << "{Block (";
        ss << s->value.size();
        ss << " items)}";
        break;
        }
      }

      isFirst = false;
    }
    ss << " -| (bottom)";
    return ss.str();
  }
}
#endif // STACK_H
