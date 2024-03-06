#pragma once

#include "chunk.h"
#include "scanner.h"
#include <memory>

namespace compiler {
class Compiler {
public:
    explicit Compiler(scanner::Scanner scanner);

    bool compile(std::shared_ptr<chunk::Chunk> chunk);

private:
    scanner::Scanner m_scanner;
};
} // namespace compiler
