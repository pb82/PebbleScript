CXX       = g++
LIBS      = -L/usr/lib -lreadline
INCPATH   = -I../../src/Include -I../../src/Include/Shared
CXXFLAGS  = -pipe -mtune=generic -O2 -pipe -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 -Wall -W -D_REENTRANT -fPIC
LFLAGS		= -shared
BIN       = libreadline.so

SOURCES		= readline.cpp

all:
	@make clean
	@$(CXX) $(CXXFLAGS) $(LFLAGS) $(INCPATH) $(LIBS) $(SOURCES) -o $(BIN)
	
clean:
	@rm -f $(BIN)*
	@rm -f *.o

