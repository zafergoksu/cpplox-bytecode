#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "utility.h"
#include <memory>
#include <utility>

using namespace token;
using namespace chunk;

namespace compiler {

Compiler::Compiler(scanner::Scanner scanner)
    : m_scanner(std::move(scanner)) {}

bool Compiler::compile(std::shared_ptr<Chunk> chunk) {
    usize line = -1;

    while (true) {
        Token token = m_scanner.scan_token();
        if (token.get_line() != line) {
            print("{:4d}", token.get_line());
            line = token.get_line();
        } else {
            print("   | ");
        }

        println("{:2d} {}", static_cast<usize>(token.get_type()), token.get_lexeme());

        if (token.get_type() == TokenType::TOKEN_EOF) {
            break;
        }
    }
    return true;
}

} // namespace compiler
