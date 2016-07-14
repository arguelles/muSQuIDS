# FLAGS

CFLAGS= -O3 -fPIC -std=c++11 

CFLAGS+= -I./inc `pkg-config --cflags nusquids`
LDFLAGS+= `pkg-config --libs nusquids` -lsupc++

all: mains/exCross.o mains/run_musquids

mains/exCross.o : inc/exCross.h mains/exCross.cpp
	@$(CXX) $(CFLAGS) -c mains/exCross.cpp -o $@

mains/run_musquids : mains/run_musquids.cpp mains/exCross.o
	@echo Compiling run_musquids
	@$(CXX) $(CFLAGS) mains/run_musquids.cpp mains/exCross.o $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf ./mains/run_musquids

