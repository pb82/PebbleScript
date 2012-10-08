#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

#include "../include/PebbleScript.h"
#include "../include/Stdlib.h"
#include "../include/Modules.h"

void usage() {
  std::cout << "usage: pebbles [PATH TO FILE]" << std::endl;
}

bool loadSource(const char *path, std::string *source, bool ignoreFirstLine=false) {
  assert(path);
  assert(source);
  source->clear();

  std::string line;
  bool firstline = ignoreFirstLine ? true : false;
  std::ifstream in(path);
  if(in.is_open()) {
    while(in) {
      std::getline(in, line);
      if(firstline) {
        firstline = false;
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
  if (loadSource(argv[1], &source, true)) {
    PS::VM vm;
    PS::Stdlib::install(vm);
    PS::Modules::install(vm);
    
    vm.eval(source.c_str());
  } else {
    std::cerr << "Failed to load " << argv[1] << std::endl;
  }
  return 0;
}

