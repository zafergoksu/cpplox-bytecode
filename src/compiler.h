#pragma once

#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "token.h"
#include "value.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace compiler {

struct Parser {
    token::Token m_current;
    token::Token m_previous;
    bool m_had_error;
    bool m_panic_mode;
};

enum class Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
};

using ParseFn = std::function<void()>;

struct ParseRule {
    std::optional<ParseFn> m_prefix;
    std::optional<ParseFn> m_infix;
    Precedence m_precedence;
};

class Compiler {
public:
    Compiler(std::shared_ptr<scanner::Scanner> scanner, std::shared_ptr<chunk::Chunk> chunk);

    bool compile();

private:
    void advance();
    void expression();
    void grouping();
    void unary();
    void number();
    void binary();
    void literal();
    void consume(token::TokenType token_type, const std::string& message);
    void parse_precedence(Precedence precedence);
    const ParseRule& get_rule(token::TokenType token_type);

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

    std::unordered_map<token::TokenType, ParseRule> m_rules{
        {token::TokenType::TOKEN_LEFT_PAREN, {std::bind(&Compiler::grouping, this), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_RIGHT_PAREN, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_LEFT_BRACE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_RIGHT_BRACE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_COMMA, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_DOT, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_MINUS, {std::bind(&Compiler::unary, this), std::bind(&Compiler::binary, this), Precedence::PREC_TERM}},
        {token::TokenType::TOKEN_PLUS, {std::nullopt, std::bind(&Compiler::binary, this), Precedence::PREC_TERM}},
        {token::TokenType::TOKEN_SEMICOLON, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_SLASH, {std::nullopt, std::bind(&Compiler::binary, this), Precedence::PREC_FACTOR}},
        {token::TokenType::TOKEN_STAR, {std::nullopt, std::bind(&Compiler::binary, this), Precedence::PREC_FACTOR}},
        {token::TokenType::TOKEN_BANG, {std::bind(&Compiler::unary, this), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_BANG_EQUAL, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_EQUAL, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_EQUAL_EQUAL, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_GREATER, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_GREATER_EQUAL, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_LESS, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_LESS_EQUAL, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_IDENTIFIER, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_STRING, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_NUMBER, {std::bind(&Compiler::number, this), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_AND, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_CLASS, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_ELSE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_FALSE, {std::bind(&Compiler::literal, this), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_FOR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_FUN, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_IF, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_NIL, {std::bind(&Compiler::literal, this), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_OR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_PRINT, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_RETURN, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_SUPER, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_THIS, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_TRUE, {std::bind(&Compiler::literal, this), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_VAR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_WHILE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_ERROR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_EOF, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},

    };
};
} // namespace compiler
