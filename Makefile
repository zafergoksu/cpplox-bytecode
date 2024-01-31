.PHONY: clear_release make_release build_release test_release full_release clear_debug make_debug build_debug test_debug full_debug

clear_release:
	rm -rf build_release

make_release:
	mkdir -p build_release && cd build_release && cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -G Ninja
	ln -sf build_release/compile_commands.json .

build_release:
	cd build_release && cmake --build .

test_release:
	cd build_release/tests && ctest -C Release

full_release: make_release build_release test_debug

clear_debug:
	rm -rf build_debug

make_debug:
	mkdir -p build_debug && cd build_debug && cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -G Ninja
	ln -sf build_debug/compile_commands.json .

build_debug:
	cd build_debug && cmake --build .

test_debug:
	cd build_debug/tests && ctest -C Debug

full_debug: make_debug build_debug test_debug

format_src:
	find src/ -regex '.*\.\(cpp\|hpp\|h\|cc\|cxx\)' -exec clang-format -style=file:"./.clang-format" -i {} \;

format_include:
	find include/ -regex '.*\.\(cpp\|hpp\|h\|cc\|cxx\)' -exec clang-format -style=file:"./.clang-format" -i {} \;

format_all: format_include format_src
