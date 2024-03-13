#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "scanner.h"
#include "token.h"
#include "utility.h"
#include "value.h"
#include <cstdint>
#include <functional>
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

const ParseRule& Compiler::get_rule(token::TokenType token_type) {
    return m_rules[token_type];
}

void Compiler::expression() {
    // Parse lowest precedence as higher precedence operators will
    // take over
    parse_precedence(Precedence::PREC_ASSIGNMENT);
}

void Compiler::grouping() {
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary() {
    TokenType operator_type = m_parser.m_previous.get_type();

    // Compile the operand, only compile the expression immediately to the right of
    // the unary operator and not the entire expression
    // (ignore +, /, etc as they have lower precedence)
    parse_precedence(Precedence::PREC_UNARY);

    // Emit the operator instruction
    switch (operator_type) {
    case TokenType::TOKEN_BANG:
        emit_byte(OpCode::OP_NOT);
        break;
    case TokenType::TOKEN_MINUS:
        emit_byte(OpCode::OP_NEGATE);
        break;
    default:
        return;
    }
}

void Compiler::binary() {
    TokenType operator_type = m_parser.m_previous.get_type();
    ParseRule rule = get_rule(operator_type);
    Precedence current_precedence = static_cast<Precedence>(static_cast<int>(rule.m_precedence) + 1);
    parse_precedence(current_precedence);

    switch (operator_type) {
    case TokenType::TOKEN_BANG_EQUAL:
        emit_bytes(OpCode::OP_EQUAL, OpCode::OP_NOT);
        break;
    case TokenType::TOKEN_EQUAL_EQUAL:
        emit_byte(OpCode::OP_EQUAL);
        break;
    case TokenType::TOKEN_GREATER:
        emit_byte(OpCode::OP_GREATER);
        break;
    case TokenType::TOKEN_GREATER_EQUAL:
        emit_bytes(OpCode::OP_LESS, OP_NOT);
        break;
    case TokenType::TOKEN_LESS:
        emit_byte(OpCode::OP_LESS);
        break;
    case TokenType::TOKEN_LESS_EQUAL:
        emit_bytes(OpCode::OP_GREATER, OP_NOT);
        break;
    case TokenType::TOKEN_PLUS:
        emit_byte(OpCode::OP_ADD);
        break;
    case TokenType::TOKEN_MINUS:
        emit_byte(OpCode::OP_SUBTRACT);
        break;
    case TokenType::TOKEN_STAR:
        emit_byte(OpCode::OP_MULTIPLY);
        break;
    case TokenType::TOKEN_SLASH:
        emit_byte(OpCode::OP_DIVIDE);
        break;
    default:
        return;
    }
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

void Compiler::literal() {
    switch (m_parser.m_previous.get_type()) {
    case TokenType::TOKEN_FALSE:
        emit_byte(OpCode::OP_FALSE);
        break;
    case TokenType::TOKEN_NIL:
        emit_byte(OpCode::OP_NIL);
        break;
    case TokenType::TOKEN_TRUE:
        emit_byte(OpCode::OP_TRUE);
        break;
    default:
        // Unreachable
        return;
    }
}

void Compiler::string() {
    std::string str = m_parser.m_previous.get_lexeme().substr(1, m_parser.m_previous.get_lexeme().length() - 2);
    emit_constant(std::move(str));
}

void Compiler::consume(TokenType token_type, const std::string& message) {
    if (m_parser.m_current.get_type() == token_type) {
        advance();
        return;
    }

    error_at_current(message);
}

void Compiler::parse_precedence(Precedence precedence) {
    advance();

    std::optional<ParseFn> prefix_rule = get_rule(m_parser.m_previous.get_type()).m_prefix;

    if (!prefix_rule) {
        error("Expect expression.");
        return;
    }

    (prefix_rule.value())();

    // keep parsing, looking for infix parse rules if the token has higher precedence than the current one
    // if we find a infix rule that has higher precedence then finish current parse and emit bytes,
    // as in the rule parsed now takes precedence over the current parse rule so continue looking.
    while (precedence <= get_rule(m_parser.m_current.get_type()).m_precedence) {
        // if the current token is say a + (with an infix parse rule), we advance and parse.
        // in this case, we go to binary parse rule which we then come back here, going to prefix parse rule
        // for number, thus we are able to emit 2 constant bytes and then a binary op
        advance();
        std::optional<ParseFn> infix_rule = get_rule(m_parser.m_previous.get_type()).m_infix;
        (infix_rule.value())();
    }
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

#ifdef DEBUG_PRINT_CODE
    if (!m_parser.m_had_error) {
        disassemble_chunk(*m_chunk, "code");
    }
#endif
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
