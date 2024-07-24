.PHONY: build
make build:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S . -B build
	cmake --build build -- -j$(shell nproc)
