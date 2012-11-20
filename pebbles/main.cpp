#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <dlfcn.h>

#include "../include/PebbleScript.h"
#include "../include/Stdlib.h"
#include "include/ModuleFinder.h"

void usage() {
  std::cout << "usage: pebbles [PATH TO FILE]" << std::endl;
}

/**
 * Allow the scripts to load external resources (shared libs)
 */
void require(PS::Environment *env) {
  if (env->expect(PS::String_T)) {    
    std::string name = env->pop<std::string>();
    
    ModuleFinder finder;
    as_module_t module = finder.lookup(name);

    if (module.type == As_T_Invalid) {
      std::ostringstream ss;
      ss << "A module with the name '";
      ss << name;
      ss << "' cound not be found";
      env->raise(ss.str().c_str());
      return;
    }
    
    void *handle = dlopen(module.path.c_str(), RTLD_NOW | RTLD_GLOBAL);
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
      PS::ExternalFunction installHook = (PS::ExternalFunction) dlsym(handle, "install");
      
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

bool loadSource(const char *path, std::string *source) {
  assert(path);
  assert(source);
  source->clear();

  std::string line;
  std::ifstream in(path);
  if(in.is_open()) {
    while(in) {
      std::getline(in, line);
      
      // Skip interpreter invocation
      if (line.substr(0,2).compare(std::string("#!")) == 0) {
        continue;
      }

      source->append(line);
      source->append("\n");
    }
  } else {
    return false;
  }

  return true;
}

int main(int argc, char *argv[])
{
  if (argc <= 1) {
    usage();
    return 0;
  }

  std::string source;
  if (loadSource(argv[1], &source)) {
    PS::VM vm;
    PS::Stdlib::install(vm);
    
    vm.def("require", require);
    
    if (!vm.eval(source.c_str())) {
      std::cerr << vm.getError() << std::endl;
    }
  } else {
    std::cerr << "Failed to load " << argv[1] << std::endl;
  }
  return 0;
}

