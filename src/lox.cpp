#include "lox.h"
#include "compiler.h"
#include "utility.h"
#include "vm.h"
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace lox {

vm::InterpretResult interpret(std::string&& source) {
    compiler::compile(std::move(source));
    return vm::InterpretResult::INTERPRET_OK;
}

void run_file(const std::string& path) {
    std::ifstream input_file{path, std::ios::binary};

    if (!input_file.is_open()) {
        println("Failed to open file");
        return;
    }

    std::string source{std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>()};
    vm::InterpretResult result = interpret(std::move(source));

    if (result == vm::InterpretResult::INTERPRET_COMPILE_ERROR) {
        exit(65);
    }

    if (result == vm::InterpretResult::INTERPRET_RUNTIME_ERROR) {
        exit(70);
    }
}

void repl() {
    std::string line;

    while (true) {
        print("> ");

        std::getline(std::cin, line);
        if (line.empty()) {
            println("");
            break;
        }
        interpret(std::move(line));
    }
}

void startup(int argc, const char* argv[]) {
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        println("Usage: clox [path]");
        exit(64);
    }
}

} // namespace lox
