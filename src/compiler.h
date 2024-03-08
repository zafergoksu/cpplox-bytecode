#pragma once

#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "token.h"
#include "value.h"
#include <memory>
#include <string>

namespace compiler {

struct Parser {
    token::Token m_current;
    token::Token m_previous;
    bool m_had_error;
    bool m_panic_mode;
};

class Compiler {
public:
    Compiler(std::shared_ptr<scanner::Scanner> scanner, std::shared_ptr<chunk::Chunk> chunk);

    bool compile();

private:
    void advance();
    void expression();
    void number();
    void consume(token::TokenType token_type, const std::string& message);

    void emit_byte(u8 byte);
    void emit_bytes(u8 byte_1, u8 byte_2);
    void emit_constant(value::Value value);
    void emit_return();
    void end_compilation();
    u8 make_constant(value::Value value);

    void error_at_current(const std::string& message);
    void error(const std::string& message);
    void error_at(const token::Token& token, const std::string& message);

    Parser m_parser;
    std::shared_ptr<scanner::Scanner> m_scanner;
    std::shared_ptr<chunk::Chunk> m_chunk;
};
} // namespace compiler
