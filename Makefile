# FLAGS
CFLAGS= -O3 -fPIC -std=c++11
CFLAGS+= -I./inc `pkg-config --cflags nusquids`
LDFLAGS+= `pkg-config --libs nusquids`

mains/run_musquids : mains/run_musquids.cpp
	@echo Compiling run_musquids
	@$(CXX) $(CFLAGS) mains/run_musquids.cpp $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf ./mains/run_musquids

