#ifndef OPERATION_H
#define OPERATION_H

#include "Type.h"

namespace PS {
  /**
   * @brief VM opcodes.
   * The possible opcodes understood by the vm.
   */
  enum Opcode {
    Push_OC,
    Call_OC,
    Plus_OC,
    Minus_OC,
    Dup_OC,
    Swap_OC,
    If_OC
  };

  /**
   * @brief Represents a vm operation.
   * There are only two possible operations: push and call.
   */
  class Operation {
    public:
      Operation (Opcode o, Type *v) : opcode(o), value(v) { }

      Opcode opcode;
      Type *value;
    };
}

#endif // OPERATION_H
