#ifndef RUNNABLE_H
#define RUNNABLE_H

#include "Types.h"

namespace PS {
  class Environment;
  typedef void (*ExternalFunction)(Environment *);

  class Runnable {
  public:
    virtual ~Runnable() { }
    virtual bool run(Block *block) = 0;
    virtual void def(const char *name, ExternalFunction def) = 0;
  };
}

#endif // RUNNABLE_H
