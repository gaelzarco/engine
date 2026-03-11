UNAME := $(shell uname)

build:
	mkdir -p build

ifeq ($(UNAME), Darwin)
CXX := $(shell brew --prefix llvm)/bin/clang++

obj: build
	$(CXX) -std=c++23 -x c++-module --precompile ./modules/obj.ixx -o build/obj.pcm

engine: build obj
	$(CXX) -std=c++23 -fmodule-file=obj=build/obj.pcm engine.cxx build/obj.pcm -o build/engine

else ifeq ($(UNAME), Linux)
CXX := g++

obj: build
	$(CXX) -std=c++23 -fmodules -fmodule-output ./modules/obj.ixx -c -o build/obj.o

engine: build obj
	$(CXX) -std=c++23 -fmodules engine.cxx build/obj.o -o build/engine

endif

clean:
	rm -rf build
	rm -rf gcm.cache
