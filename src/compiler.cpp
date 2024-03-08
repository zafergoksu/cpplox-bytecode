#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "token.h"
#include "utility.h"
#include "value.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

using namespace token;
using namespace chunk;
using namespace scanner;
using namespace value;

namespace compiler {

Compiler::Compiler(std::shared_ptr<Scanner> scanner, std::shared_ptr<Chunk> chunk)
    : m_scanner{std::move(scanner)},
      m_parser{Token{TokenType::TOKEN_EOF, "", 1},
               Token{TokenType::TOKEN_EOF, "", 1},
               false,
               false},
      m_chunk{std::move(chunk)} {}

bool Compiler::compile() {
    advance();
    expression();
    consume(TokenType::TOKEN_EOF, "Expect end of expression.");

    end_compilation();
    return !m_parser.m_had_error;
}

void Compiler::expression() {
    // TODO(zgoksu): complete compilation
    advance();
    number();
}

void Compiler::advance() {
    m_parser.m_previous = m_parser.m_current;

    while (true) {
        m_parser.m_current = m_scanner->scan_token();
        if (m_parser.m_current.get_type() != TokenType::TOKEN_ERROR) {
            break;
        }

        error_at_current(m_parser.m_current.get_lexeme());
    }
}

void Compiler::number() {
    Value value = std::stod(m_parser.m_previous.get_lexeme());
    emit_constant(value);
}

void Compiler::consume(TokenType token_type, const std::string& message) {
    if (m_parser.m_current.get_type() == token_type) {
        advance();
        return;
    }

    error_at_current(message);
}

void Compiler::emit_byte(u8 byte) {
    m_chunk->write_byte(byte, m_parser.m_previous.get_line());
}

void Compiler::emit_bytes(u8 byte_1, u8 byte_2) {
    emit_byte(byte_1);
    emit_byte(byte_2);
}

void Compiler::emit_constant(Value value) {
    emit_bytes(OpCode::OP_CONSTANT, make_constant(value));
}

void Compiler::emit_return() {
    emit_byte(OpCode::OP_RETURN);
}

void Compiler::end_compilation() {
    emit_return();
}

u8 Compiler::make_constant(Value value) {
    usize constant_idx = m_chunk->write_constant(value);
    if (constant_idx > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<u8>(constant_idx);
}

void Compiler::error_at_current(const std::string& message) {
    error_at(m_parser.m_current, message);
}

void Compiler::error(const std::string& message) {
    error_at(m_parser.m_previous, message);
}

void Compiler::error_at(const Token& token, const std::string& message) {
    if (m_parser.m_panic_mode == true) {
        return;
    }

    m_parser.m_panic_mode = true;

    print_err("[line {}] Error", token.get_line());

    if (token.get_type() == TokenType::TOKEN_EOF) {
        print_err(" at end");
    } else if (token.get_type() == TokenType::TOKEN_ERROR) {

    } else {
        print_err(" at {}", token.get_lexeme());
    }

    println_err(": {}", message);
    m_parser.m_had_error = true;
}

} // namespace compiler
