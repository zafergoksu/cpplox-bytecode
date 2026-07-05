#pragma once

#include "heap.h"
#include "vm.h"
#include <memory>
#include <string>

namespace lox {

vm::InterpretResult interpret(std::string source, vm::VirtualMachine& vm, const std::shared_ptr<ds::Heap>& heap);
void run_file(const std::string& path, vm::VirtualMachine& vm, const std::shared_ptr<ds::Heap>& heap);
void repl(vm::VirtualMachine& vm, const std::shared_ptr<ds::Heap>& heap);
void startup(int argc, const char* argv[]);

} // namespace lox
