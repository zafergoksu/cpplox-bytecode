#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "token.h"
#include "utility.h"
#include "value.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

using namespace token;
using namespace chunk;
using namespace scanner;
using namespace value;

namespace compiler {

Local::Local()
    : m_name{TokenType::TOKEN_EOF, "", 0},
      m_depth{std::nullopt} {}

Compiler::Compiler(std::shared_ptr<Scanner> scanner, std::shared_ptr<Chunk> chunk)
    : m_scanner{std::move(scanner)},
      m_parser{Token{TokenType::TOKEN_EOF, "", 1},
               Token{TokenType::TOKEN_EOF, "", 1},
               false,
               false},
      m_local_count{0},
      m_scope_depth{0},
      m_chunk{std::move(chunk)} {}

bool Compiler::compile() {
    advance();

    while (!match(TokenType::TOKEN_EOF)) {
        declaration();
    }

    end_compilation();
    return !m_parser.m_had_error;
}

const ParseRule& Compiler::get_rule(token::TokenType token_type) {
    return m_rules[token_type];
}

void Compiler::synchronize() {
    // Reset to default state as after synchronization
    // we assume further statements as valid.
    m_parser.m_panic_mode = false;

    while (m_parser.m_current.get_type() != TokenType::TOKEN_EOF) {
        // We looked a semicolon, our boundary point for statements
        if (m_parser.m_previous.get_type() == TokenType::TOKEN_SEMICOLON) {
            return;
        }

        switch (m_parser.m_current.get_type()) {
        case TokenType::TOKEN_CLASS:
        case TokenType::TOKEN_FUN:
        case TokenType::TOKEN_VAR:
        case TokenType::TOKEN_FOR:
        case TokenType::TOKEN_IF:
        case TokenType::TOKEN_WHILE:
        case TokenType::TOKEN_PRINT:
        case TokenType::TOKEN_RETURN:
            return;
        default:
            // Do nothing here.
            advance();
        }
    }
}

u8 Compiler::parse_variable(const std::string& error_msg) {
    consume(TokenType::TOKEN_IDENTIFIER, error_msg);

    declare_variable();
    if (m_scope_depth > 0) {
        // look up on stack rather than constant table to hash table value
        return 0;
    }

    return identifier_constant(m_parser.m_previous);
}

void Compiler::mark_initialized() {
    m_locals[m_local_count - 1].m_depth = m_scope_depth;
}

u8 Compiler::identifier_constant(const token::Token& token) {
    auto obj_string = std::make_shared<object::StringObject>(token.get_lexeme());
    return make_constant(std::move(obj_string));
}

std::optional<u8> Compiler::resolve_local(const Token& name) {
    for (int i = m_local_count - 1; i >= 0; i--) {
        const Local& local = m_locals[i];
        if (local.m_name.get_lexeme() == name.get_lexeme()) {
            if (local.m_depth == std::nullopt) {
                error("Can't read local variable in its own initializer.");
            }
            return i;
        }
    }

    return std::nullopt;
}

void Compiler::add_local(const Token& name) {
    if (m_local_count == UINT8_COUNT) {
        error("Too many local variables in function.");
        return;
    }
    Local& local = m_locals[m_local_count++];
    local.m_name = name;
    local.m_depth = std::nullopt;
}

void Compiler::declare_variable() {
    if (m_scope_depth == 0) {
        return;
    }

    const Token& name = m_parser.m_previous;

    for (int i = m_local_count - 1; i >= 0; i--) {
        const Local& local = m_locals[i];
        // check the local variables from the end of the array
        // to the beginning. if a variable has a depth lower than
        // the current, it is owned by the preceeding scope.
        if (local.m_depth != std::nullopt && local.m_depth.value() < m_scope_depth) {
            break;
        }

        if (name.get_lexeme() == local.m_name.get_lexeme()) {
            error("Already a variable with this name in this scope.");
        }
    }

    add_local(name);
}

void Compiler::define_variable(u8 global) {
    if (m_scope_depth > 0) {
        mark_initialized();
        return;
    }
    emit_bytes(OpCode::OP_DEFINE_GLOBAL, global);
}

void Compiler::named_variable(const token::Token& name, bool can_assign) {
    u8 get_op = 0;
    u8 set_op = 0;
    std::optional<u8> arg = resolve_local(name);

    if (arg) {
        get_op = OpCode::OP_GET_LOCAL;
        set_op = OpCode::OP_SET_LOCAL;
    } else {
        arg = identifier_constant(name);
        get_op = OpCode::OP_GET_GLOBAL;
        set_op = OpCode::OP_SET_GLOBAL;
    }

    if (can_assign && match(TokenType::TOKEN_EQUAL)) {
        expression();
        emit_bytes(set_op, arg.value());
    } else {
        emit_bytes(get_op, arg.value());
    }
}

void Compiler::begin_scope() {
    m_scope_depth++;
}

void Compiler::end_scope() {
    m_scope_depth--;

    while (m_local_count > 0 && m_locals[m_local_count - 1].m_depth != std::nullopt && m_locals[m_local_count - 1].m_depth.value() > m_scope_depth) {
        emit_byte(OpCode::OP_POP);
        m_local_count--;
    }
}

int Compiler::emit_jump(u8 instruction) {
    emit_byte(instruction);
    emit_byte(0xff);
    emit_byte(0xff);
    return m_chunk->size() - 2;
}

void Compiler::patch_jump(int offset) {
    // -2 to adjust for the bytecode for the jump offset itself.
    int jump = m_chunk->size() - offset - 2;

    if (jump > UINT16_MAX) {
        error("Too much code to jump over.");
    }

    // high byte
    m_chunk->write_byte_at(offset, (jump >> 8) & 0xff);
    // low byte
    m_chunk->write_byte_at(offset + 1, jump & 0xff);
}

bool Compiler::match(token::TokenType token_type) {
    if (!check(token_type)) {
        return false;
    }

    advance();
    return true;
}

bool Compiler::check(token::TokenType token_type) {
    return m_parser.m_current.get_type() == token_type;
}

void Compiler::statement() {
    if (match(TokenType::TOKEN_PRINT)) {
        print_statement();
    } else if (match(TokenType::TOKEN_FOR)) {
        for_statement();
    } else if (match(TokenType::TOKEN_IF)) {
        if_statement();
    } else if (match(TokenType::TOKEN_WHILE)) {
        while_statement();
    } else if (match(TokenType::TOKEN_LEFT_BRACE)) {
        begin_scope();
        block_statement();
        end_scope();
    } else {
        expression_statement();
    }
}

void Compiler::declaration() {
    if (match(TokenType::TOKEN_VAR)) {
        var_declaration();
    } else {
        statement();
    }

    // We minimise errors by preventing cascading
    // errors from the initial error. We synchronize the compiler
    // to a point where we assume valid statements further on,
    // we have our boundary points.
    if (m_parser.m_panic_mode) {
        synchronize();
    }
}

void Compiler::var_declaration() {
    u8 global = parse_variable("Expect variable name.");

    if (match(TokenType::TOKEN_EQUAL)) {
        expression();
    } else {
        // we set statements like `var a;` to nil
        emit_byte(OP_NIL);
    }

    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    define_variable(global);
}

void Compiler::print_statement() {
    expression();
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after value.");
    emit_byte(OpCode::OP_PRINT);
}

void Compiler::for_statement() {
    begin_scope();
    consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    if (match(TokenType::TOKEN_SEMICOLON)) {
        // No initializer
    } else if (match(TokenType::TOKEN_VAR)) {
        var_declaration();
    } else {
        expression_statement();
    }

    int loop_start = m_chunk->size();
    int exit_jump = -1;
    if (!match(TokenType::TOKEN_SEMICOLON)) {
        expression();
        consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after loop condition.");
    }

    // Jump out of the loop if the condition is false;
    exit_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
    emit_byte(OpCode::OP_POP);

    if (!match(TokenType::TOKEN_RIGHT_PAREN)) {
        // jump to the body of the for loop
        int body_jump = emit_jump(OpCode::OP_JUMP);
        int increment_start = m_chunk->size();
        expression();
        emit_byte(OpCode::OP_POP);
        consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        // once we are done with the body, we want to jump to the increment expression section of the code
        emit_loop(loop_start);
        // then we set the jump address to the start of the actual for loop
        loop_start = increment_start;
        // patch the body jump address as we have compiled the looping logic
        patch_jump(body_jump);
    }

    statement();
    emit_loop(loop_start);

    if (exit_jump != -1) {
        patch_jump(exit_jump);
        emit_byte(OpCode::OP_POP);
    }
    end_scope();
}

void Compiler::expression_statement() {
    expression();
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after expression.");
    emit_byte(OpCode::OP_POP);
}

void Compiler::block_statement() {
    while (!check(TokenType::TOKEN_RIGHT_BRACE) && !check(TokenType::TOKEN_EOF)) {
        declaration();
    }

    consume(TokenType::TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::if_statement() {
    consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int then_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
    // if statements should have zero stack effect, therefore, if we are in the
    // then branch we pop the condition expression immediately.
    emit_byte(OpCode::OP_POP);
    statement();

    // we jump when we are done with the 'then' statement,
    // patching the 'then' jump
    int else_jump = emit_jump(OpCode::OP_JUMP);

    patch_jump(then_jump);

    // otherwise we pop it at the beginning of the else branch
    emit_byte(OpCode::OP_POP);

    if (match(TokenType::TOKEN_ELSE)) {
        statement();
    }
    patch_jump(else_jump);
}

void Compiler::while_statement() {
    int loop_start = m_chunk->size();
    consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exit_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
    emit_byte(OpCode::OP_POP);
    statement();
    emit_loop(loop_start);

    patch_jump(exit_jump);
    emit_byte(OpCode::OP_POP);
}
void Compiler::expression() {
    // Parse lowest precedence as higher precedence operators will
    // take over
    parse_precedence(Precedence::PREC_ASSIGNMENT);
}

void Compiler::grouping(bool can_assign) {
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::unary(bool can_assign) {
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

void Compiler::binary(bool can_assign) {
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

void Compiler::number(bool can_assign) {
    auto value = std::make_shared<object::NumberObject>(std::stod(m_parser.m_previous.get_lexeme()));
    emit_constant(std::move(value));
}

void Compiler::literal(bool can_assign) {
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

void Compiler::string(bool can_assign) {
    std::string str = m_parser.m_previous.get_lexeme().substr(1, m_parser.m_previous.get_lexeme().length() - 2);
    emit_constant(std::make_shared<object::StringObject>(std::move(str)));
}

void Compiler::variable(bool can_assign) {
    named_variable(m_parser.m_previous, can_assign);
}

void Compiler::and_infix(bool can_assign) {
    // we've parsed the left-hand side of the expression (as this is
    // an infix op), the result of the left is left on top of stack
    int end_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);

    // remove left value if true
    emit_byte(OpCode::OP_POP);
    // parse right hand of expression and leave on top of stack
    parse_precedence(Precedence::PREC_AND);

    patch_jump(end_jump);
}

void Compiler::or_infix(bool can_assign) {
    /*
     *          left hand operand expression
     * .------- OP_JUMP_IF_FALSE
     * |-.----- OP_JUMP
     * .-|----- OP_POP
     *   |      right hand operand expression
     *   .----- continue
     */
    int else_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
    int end_jump = emit_jump(OpCode::OP_JUMP);

    patch_jump(else_jump);
    emit_byte(OpCode::OP_POP);

    parse_precedence(Precedence::PREC_OR);
    patch_jump(end_jump);
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

    // only parse assignments if the current precedence is at PREC_ASSIGNMENT or lower
    bool can_assign = precedence <= Precedence::PREC_ASSIGNMENT;
    (prefix_rule.value())(can_assign);

    // keep parsing, looking for infix parse rules if the current token has higher precedence than the previous one
    // if we find a infix rule that has higher precedence then parse it and emit bytes.
    while (precedence <= get_rule(m_parser.m_current.get_type()).m_precedence) {
        // recursively parse infix rules while the current parsed token has greater precedence than
        // the previously parse token
        advance();
        std::optional<ParseFn> infix_rule = get_rule(m_parser.m_previous.get_type()).m_infix;
        (infix_rule.value())(can_assign);
    }

    if (can_assign && match(TokenType::TOKEN_EQUAL)) {
        error("Invalid assignment target.");
    }
}

void Compiler::emit_byte(u8 byte) {
    m_chunk->write_byte(byte, m_parser.m_previous.get_line());
}

void Compiler::emit_bytes(u8 byte_1, u8 byte_2) {
    emit_byte(byte_1);
    emit_byte(byte_2);
}

void Compiler::emit_constant(std::shared_ptr<object::Object> value) {
    emit_bytes(OpCode::OP_CONSTANT, make_constant(std::move(value)));
}

void Compiler::emit_return() {
    emit_byte(OpCode::OP_RETURN);
}

void Compiler::emit_loop(int loop_start) {
    emit_byte(OpCode::OP_LOOP);

    int offset = m_chunk->size() - loop_start + 2;
    if (offset > UINT16_MAX) {
        error("Loop body too large.");
    }

    emit_byte((offset >> 8) & 0xff);
    emit_byte(offset & 0xff);
}

void Compiler::end_compilation() {
    emit_return();

#ifdef DEBUG_PRINT_CODE
    if (!m_parser.m_had_error) {
        disassemble_chunk(*m_chunk, "code");
    }
#endif
}

u8 Compiler::make_constant(std::shared_ptr<object::Object> value) {
    usize constant_idx = m_chunk->write_constant(std::move(value));
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
