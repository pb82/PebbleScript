#ifndef FREESTORE_H
#define FREESTORE_H

#include <deque>

#include "Types.h"

namespace PS {
  template <typename T> class FreeStore {
  public:
    T *get() {
      if (store.empty()) {
        return 0;
      } else {
        T *p = store.front();
        store.pop_front();
        return p;
      }
    }

    void destroy(T *p) {
      store.push_back(p);
    }

  private:
    std::deque<T *> store;
  };
}

#endif // FREESTORE_H
