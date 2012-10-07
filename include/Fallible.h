#ifndef FALLIBLE_H
#define FALLIBLE_H

#include <string>
#include <iostream>

namespace PS {
  class Fallible {
  public:
    Fallible() : runtimeErrorOccured(false) { }
    virtual ~Fallible() { }

    void raise(const char *msg);
    void raise(std::string &msg);

    bool runtimeErrorOccured;
    std::string runtimeError;
  };

  inline void Fallible::raise(const char *msg) {
    runtimeError = std::string(msg);
    runtimeErrorOccured = true;
  }
}
#endif // FALLIBLE_H
