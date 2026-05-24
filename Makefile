.PHONY: clear_release make_release build_release test_release full_release clear_debug make_debug build_debug test_debug full_debug format_src format_include format_all

clear_release:
	rm -rf build/release

make_release:
	cmake --preset release
	ln -sf build/release/compile_commands.json .

build_release:
	cmake --build --preset release

test_release:
	cd build/release/tests && ctest --output-on-failure

full_release: make_release build_release test_release

clear_debug:
	rm -rf build/debug

make_debug:
	cmake --preset debug
	ln -sf build/debug/compile_commands.json .

build_debug:
	cmake --build --preset debug

test_debug:
	cd build/debug/tests && ctest --output-on-failure

full_debug: make_debug build_debug test_debug

format_src:
	find src/ -regex '.*\.\(cpp\|hpp\|h\|cc\|cxx\)' -exec clang-format -style=file:"./.clang-format" -i {} \;

format_include:
	find include/ -regex '.*\.\(cpp\|hpp\|h\|cc\|cxx\)' -exec clang-format -style=file:"./.clang-format" -i {} \;

format_all: format_include format_src
