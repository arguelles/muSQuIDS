# FLAGS

CFLAGS= -O3 -fPIC -std=c++11
CFLAGS+= -I./inc `pkg-config --cflags nusquids`
LDFLAGS+= `pkg-config --libs nusquids` -lrt -lsupc++
#LDFLAGS+=-lcxxrt

ifeq ($(UNAME_S),Linux)
	LDFLAGS+=-lsupc++
endif

all: mains/exCross.o mains/load_flux.o mains/run_musquids mains/run_nusquids mains/run_musquids_simple mains/run_musquids_less_simple

mains/exCross.o : inc/exCross.h mains/exCross.cpp
	@echo Compiling exCross
	@$(CXX) $(CFLAGS) -c mains/exCross.cpp -o $@

mains/load_flux.o : inc/loadFlux.h mains/load_flux.cpp
	@echo Compiling load_flux
	@$(CXX) $(CFLAGS) -c mains/load_flux.cpp -o $@

mains/run_musquids : mains/run_musquids.cpp mains/exCross.o mains/load_flux.o
	@echo Compiling run_musquids
	@$(CXX) $(CFLAGS) mains/run_musquids.cpp mains/exCross.o mains/load_flux.o $(LDFLAGS) -o $@

mains/run_nusquids : mains/run_musquids.cpp mains/exCross.o mains/load_flux.o
	@echo Compiling run_nusquids
	@$(CXX) $(CFLAGS) mains/run_nusquids.cpp mains/exCross.o mains/load_flux.o $(LDFLAGS) -o $@

mains/run_musquids_simple : mains/run_musquids_simple.cpp mains/exCross.o
	@echo Compiling run_musquids_simple
	@$(CXX) $(CFLAGS) mains/run_musquids_simple.cpp mains/exCross.o $(LDFLAGS) -o $@

mains/run_musquids_less_simple : mains/run_musquids_less_simple.cpp mains/exCross.o
	@echo Compiling run_musquids_less_simple
	@$(CXX) $(CFLAGS) mains/run_musquids_less_simple.cpp mains/exCross.o $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf ./mains/*.o ./mains/run_nusquids ./mains/run_musquids ./mains/run_musquids_simple ./mains/run_musquids_less_simple



