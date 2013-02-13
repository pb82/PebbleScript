CXX				= g++
INCPATH   = -I../include
LIBS      = -L/usr/lib -ldl
CXXFLAGS	= -pipe -mtune=generic -O2 -pipe -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 -Wall -W -D_REENTRANT
BIN				=	pebbles
SOURCES		= main.cpp ModuleFinder.cpp

all:
	@$(CXX) $(CXXFLAGS) $(INCPATH) $(LIBS) $(SOURCES) -o $(BIN)

install:
	@cp -p $(BIN) /usr/local/bin

clean:
	@rm -f $(BIN)
	@rm -f *.o
