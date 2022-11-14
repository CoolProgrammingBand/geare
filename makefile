compile-debug: prelude
	clang++ ./main.cpp -o ./build/debug/main -g -Wall -std=c++20 -include-pch ./build/pch/entt.pch ./build/pch/glm.pch ./build/pch/glfw.pch ./build/pch/gtest.pch -lglfw -lGL -I ./ext/

warmup: prelude compile-ext-pch

compile-ext-pch: 
	mkdir -p ./build/pch
	clang++ -x c++-header -stdlib=libstdc++ ./ext/entt.hpp -o ./build/pch/entt.pch -std=c++20
	clang++ -x c++-header -stdlib=libstdc++ ./ext/glm.hpp -o ./build/pch/glm.pch -std=c++20
	clang++ -x c++-header -stdlib=libstdc++ ./ext/glfw.hpp -o ./build/pch/glfw.pch -std=c++20
	clang++ -x c++-header -stdlib=libstdc++ ./ext/gtest.hpp -o ./build/pch/gtest.pch -std=c++20 -I./ext/gtest/googletest/include

prelude:
	mkdir -p ./build/debug
	mkdir -p ./build/release

clean:
	rm -rf ./build
