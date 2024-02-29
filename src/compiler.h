#pragma once

#include "scanner.h"

namespace compiler {
class Compiler {
public:
    explicit Compiler(scanner::Scanner scanner);

    void compile();

private:
    scanner::Scanner m_scanner;
};
} // namespace compiler
