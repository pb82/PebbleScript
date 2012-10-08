#ifndef MODULES_H
#define MODULES_H

#include <dlfcn.h>
#include <sstream>
#include <iostream>

#include "PebbleScript.h"

namespace PS { namespace Modules {
  /**
   * @brief require allows a script to load native libraries.
   * The libraries must define an 'install' function with a
   * signature of void (Environment *).
   * @param env pointer to the runtime environment
   */
  void require(Environment *env) {
    if (env->expect(String_T)) {
      std::string name = env->pop<std::string>();
      
      void *handle = dlopen(name.c_str(), RTLD_NOW | RTLD_GLOBAL);
      if (!handle) {
        std::ostringstream ss;
        ss << "Failed to load native module '";
        ss << name;
        ss << "'";
        env->raise(ss.str().c_str());
        return;
      } else {
        // Reset errors
        dlerror();

        // Find the install function
        ExternalFunction installHook = (ExternalFunction) dlsym(handle, "install");

        // Check for errors again
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
          std::cout << "error: " << dlsym_error << std::endl;
          std::ostringstream ss;
          ss << "Native module '";
          ss << name;
          ss << "' is corrupt";
          env->raise(ss.str().c_str());
          return;
        } else {
          installHook(env);
        }
      }
    }
  }

  void install(VM &vm) {
    vm.def("require", require);
  }
} }

#endif // MODULES_H
