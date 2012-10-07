#ifndef RUNNABLE_H
#define RUNNABLE_H

#include "Types.h"

namespace PS {
  class Runnable {
  public:
    virtual ~Runnable() { }
    virtual bool run(Block *block) = 0;
  };
}

#endif // RUNNABLE_H
