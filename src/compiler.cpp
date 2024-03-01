#include "compiler.h"
#include "common.h"
#include "scanner.h"
#include "token.h"
#include "utility.h"
#include <utility>

using namespace token;

namespace compiler {

Compiler::Compiler(scanner::Scanner scanner)
    : m_scanner(std::move(scanner)) {}

void Compiler::compile() {
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
}

} // namespace compiler
