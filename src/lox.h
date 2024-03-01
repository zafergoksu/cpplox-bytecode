#pragma once

#include "vm.h"
#include <string>

namespace lox {

vm::InterpretResult interpret(std::string source);
void run_file(const std::string& path);
void repl();
void startup(int argc, const char* argv[]);

} // namespace lox
