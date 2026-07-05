#include "lox.h"
#include "chunk.h"
#include "compiler.h"
#include "heap.h"
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
using namespace object;

using ds::Heap;

namespace lox {

vm::InterpretResult interpret(std::string source, vm::VirtualMachine& vm, const std::shared_ptr<ds::Heap>& heap) {
    auto scanner = std::make_shared<Scanner>(std::move(source));
    Compiler compiler{scanner, heap, object::FunctionType::TYPE_SCRIPT};

    FunctionObject* function = compiler.compile();
    if (function == nullptr) {
        return vm::InterpretResult::INTERPRET_COMPILE_ERROR;
    }

    vm.load_function(function, heap);
    return vm.run();
}

void run_file(const std::string& path, vm::VirtualMachine& vm, const std::shared_ptr<ds::Heap>& heap) {
    std::ifstream input_file{path, std::ios::binary};

    if (!input_file.is_open()) {
        println("Failed to open file");
        return;
    }

    std::string source{std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>()};
    vm::InterpretResult result = interpret(std::move(source), vm, heap);

    if (result == vm::InterpretResult::INTERPRET_COMPILE_ERROR) {
        exit(65);
    }

    if (result == vm::InterpretResult::INTERPRET_RUNTIME_ERROR) {
        exit(70);
    }
}

void repl(vm::VirtualMachine& vm, const std::shared_ptr<ds::Heap>& heap) {
    std::string line;

    while (true) {
        print("> ");

        std::getline(std::cin, line);
        if (line.empty()) {
            println("");
            break;
        }
        interpret(std::move(line), vm, heap);
    }
}

void startup(int argc, const char* argv[]) {
    vm::VirtualMachine vm;
    auto heap = std::make_shared<ds::Heap>();
    if (argc == 1) {
        repl(vm, heap);
    } else if (argc == 2) {
        run_file(argv[1], vm, heap);
    } else {
        println("Usage: clox [path]");
        exit(64);
    }
}

} // namespace lox
