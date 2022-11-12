compile-debug: prelude
	clang++ ./main.cpp -o ./build/debug/main -g -Wall -std=c++20

prelude:
	mkdir -p ./build/debug
	mkdir -p ./build/release

clean:
	rm -rf ./build
