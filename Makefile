.PHONY: all
all: format build

.PHONY: format
format:
	clang-format src/*.cpp include/* -i
	clang-format tests/*.cpp include/* -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

thirdparty/googletest/CMakeLists.txt:
	git submodule update --init --recursive

.PHONY: test
test: thirdparty/googletest/CMakeLists.txt
	mkdir -p build
	cd build && \
	cmake -DBUILD_TESTS=ON .. && \
	make

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: clean
clean:
	rm -rf build
