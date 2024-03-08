#include "lox.h"
#include "chunk.h"
#include "compiler.h"
#include "scanner.h"
#include "utility.h"
#include "vm.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

using namespace compiler;
using namespace scanner;
using namespace chunk;

namespace lox {

vm::InterpretResult interpret(std::string source, vm::VirtualMachine& vm) {
    auto scanner = std::make_shared<Scanner>(std::move(source));
    auto chunk = std::make_shared<Chunk>();
    Compiler compiler{scanner, chunk};

    if (!compiler.compile()) {
        return vm::InterpretResult::INTERPRET_COMPILE_ERROR;
    }

    vm.load_new_chunk(chunk);
    vm.run();

    return vm::InterpretResult::INTERPRET_OK;
}

void run_file(const std::string& path, vm::VirtualMachine& vm) {
    std::ifstream input_file{path, std::ios::binary};

    if (!input_file.is_open()) {
        println("Failed to open file");
        return;
    }

    std::string source{std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>()};
    vm::InterpretResult result = interpret(std::move(source), vm);

    if (result == vm::InterpretResult::INTERPRET_COMPILE_ERROR) {
        exit(65);
    }

    if (result == vm::InterpretResult::INTERPRET_RUNTIME_ERROR) {
        exit(70);
    }
}

void repl(vm::VirtualMachine& vm) {
    std::string line;

    while (true) {
        print("> ");

        std::getline(std::cin, line);
        if (line.empty()) {
            println("");
            break;
        }
        interpret(std::move(line), vm);
    }
}

void startup(int argc, const char* argv[]) {
    vm::VirtualMachine vm;
    if (argc == 1) {
        repl(vm);
    } else if (argc == 2) {
        run_file(argv[1], vm);
    } else {
        println("Usage: clox [path]");
        exit(64);
    }
}

} // namespace lox
