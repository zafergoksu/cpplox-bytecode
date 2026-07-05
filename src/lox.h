#pragma once

#include "vm.h"
#include <string>

namespace lox {

vm::InterpretResult interpret(std::string source, vm::VirtualMachine& vm);
void run_file(const std::string& path, vm::VirtualMachine& vm);
void repl(vm::VirtualMachine& vm);
void startup(int argc, const char* argv[]);

} // namespace lox
