.PHONY: build
build:
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S . -B build
	cmake --build build -- -j$(shell nproc)

.PHONY: test
test:
	build/hydro inputs/test.hy -o outputs/actual.asm
	diff outputs/actual.asm outputs/test.asm
