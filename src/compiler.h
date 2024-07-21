#pragma once

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "scanner.h"
#include "token.h"
#include "value.h"
#include <array>
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

struct Local {
    Local();
    token::Token m_name;
    std::optional<u8> m_depth;
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

using ParseFn = std::function<void(bool)>;

struct ParseRule {
    std::optional<ParseFn> m_prefix;
    std::optional<ParseFn> m_infix;
    Precedence m_precedence;
};

class Compiler {
public:
    Compiler(std::shared_ptr<scanner::Scanner> scanner, object::FunctionType type);

    std::shared_ptr<object::FunctionObject> compile();

private:
    void advance();
    bool match(token::TokenType token_type);
    bool check(token::TokenType token_type);

    // statements
    void statement();
    void declaration();
    void var_declaration();
    void print_statement();
    void for_statement();
    void expression_statement();
    void block_statement();
    void if_statement();
    void while_statement();

    // expressions
    void expression();
    void grouping(bool can_assign);
    void unary(bool can_assign);
    void number(bool can_assign);
    void binary(bool can_assign);
    void literal(bool can_assign);
    void string(bool can_assign);
    void variable(bool can_assign);
    void and_infix(bool can_assign);
    void or_infix(bool can_assign);

    void consume(token::TokenType token_type, const std::string& message);
    void parse_precedence(Precedence precedence);
    const ParseRule& get_rule(token::TokenType token_type);
    void synchronize();
    u8 parse_variable(const std::string& error_msg);
    void mark_initialized();
    u8 identifier_constant(const token::Token& token);
    std::optional<u8> resolve_local(const token::Token& name);
    void add_local(const token::Token& name);
    void declare_variable();
    void define_variable(u8 global);
    void named_variable(const token::Token& name, bool can_assign);
    void begin_scope();
    void end_scope();
    int emit_jump(u8 instruction);
    void patch_jump(int offset);

    void emit_byte(u8 byte);
    void emit_bytes(u8 byte_1, u8 byte_2);
    void emit_constant(std::shared_ptr<object::Object> value);
    void emit_return();
    std::shared_ptr<object::FunctionObject> end_compilation();
    void emit_loop(int loop_start);
    u8 make_constant(std::shared_ptr<object::Object> value);

    void error_at_current(const std::string& message);
    void error(const std::string& message);
    void error_at(const token::Token& token, const std::string& message);

    const chunk::Chunk& current_chunk() const;

    Parser m_parser;
    std::array<Local, UINT8_COUNT> m_locals;
    int m_local_count;
    int m_scope_depth;
    std::shared_ptr<scanner::Scanner> m_scanner;
    std::shared_ptr<object::FunctionObject> m_function;

    std::unordered_map<token::TokenType, ParseRule> m_rules{
        {token::TokenType::TOKEN_LEFT_PAREN, {std::bind(&Compiler::grouping, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_RIGHT_PAREN, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_LEFT_BRACE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_RIGHT_BRACE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_COMMA, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_DOT, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_MINUS, {std::bind(&Compiler::unary, this, std::placeholders::_1), std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_TERM}},
        {token::TokenType::TOKEN_PLUS, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_TERM}},
        {token::TokenType::TOKEN_SEMICOLON, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_SLASH, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_FACTOR}},
        {token::TokenType::TOKEN_STAR, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_FACTOR}},
        {token::TokenType::TOKEN_BANG, {std::bind(&Compiler::unary, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_BANG_EQUAL, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_EQUALITY}},
        {token::TokenType::TOKEN_EQUAL, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_EQUAL_EQUAL, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_EQUALITY}},
        {token::TokenType::TOKEN_GREATER, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_COMPARISON}},
        {token::TokenType::TOKEN_GREATER_EQUAL, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_COMPARISON}},
        {token::TokenType::TOKEN_LESS, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_COMPARISON}},
        {token::TokenType::TOKEN_LESS_EQUAL, {std::nullopt, std::bind(&Compiler::binary, this, std::placeholders::_1), Precedence::PREC_COMPARISON}},
        {token::TokenType::TOKEN_IDENTIFIER, {std::bind(&Compiler::variable, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_STRING, {std::bind(&Compiler::string, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_NUMBER, {std::bind(&Compiler::number, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_AND, {std::nullopt, std::bind(&Compiler::and_infix, this, std::placeholders::_1), Precedence::PREC_AND}},
        {token::TokenType::TOKEN_CLASS, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_ELSE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_FALSE, {std::bind(&Compiler::literal, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_FOR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_FUN, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_IF, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_NIL, {std::bind(&Compiler::literal, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_OR, {std::nullopt, std::bind(&Compiler::or_infix, this, std::placeholders::_1), Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_PRINT, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_RETURN, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_SUPER, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_THIS, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_TRUE, {std::bind(&Compiler::literal, this, std::placeholders::_1), std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_VAR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_WHILE, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_ERROR, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
        {token::TokenType::TOKEN_EOF, {std::nullopt, std::nullopt, Precedence::PREC_NONE}},
    };
};
} // namespace compiler
