#include "scanner.h"
#include "token.h"
#include <string>
#include <utility>

using namespace token;

namespace scanner {

Scanner::Scanner()
    : m_start{0},
      m_current{0},
      m_line{1} {}

Scanner::Scanner(std::string source)
    : m_source{std::move(source)},
      m_start{0},
      m_current{0},
      m_line{1} {}

void Scanner::load_source(std::string source) {
    m_source = std::move(source);
}

Token Scanner::scan_token() {
    skip_whitespace();
    m_start = m_current;
    if (is_at_end()) {
        return Token{TokenType::TOKEN_EOF, "", m_line};
    }

    char c = advance();

    switch (c) {
    case '(':
        return make_token(TokenType::TOKEN_LEFT_PAREN);
    case ')':
        return make_token(TokenType::TOKEN_RIGHT_PAREN);
    case '{':
        return make_token(TokenType::TOKEN_LEFT_BRACE);
    case '}':
        return make_token(TokenType::TOKEN_RIGHT_BRACE);
    case ';':
        return make_token(TokenType::TOKEN_SEMICOLON);
    case ',':
        return make_token(TokenType::TOKEN_COMMA);
    case '.':
        return make_token(TokenType::TOKEN_DOT);
    case '-':
        return make_token(TokenType::TOKEN_MINUS);
    case '+':
        return make_token(TokenType::TOKEN_PLUS);
    case '/':
        return make_token(TokenType::TOKEN_SLASH);
    case '*':
        return make_token(TokenType::TOKEN_STAR);
    case '!':
        return make_token(match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
    case '=':
        return make_token(match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
    case '<':
        return make_token(match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
    case '>':
        return make_token(match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
    case '"':
        return string();
    }

    return make_error_token("Unexpected character.");
}

Token Scanner::make_token(TokenType token_type) {
    return Token{token_type, m_source.substr(m_start, m_current - m_start), m_line};
}

Token Scanner::make_error_token(std::string error_message) {
    return Token{TokenType::TOKEN_ERROR, std::move(error_message), m_line};
}

Token Scanner::string() {
    while (!is_at_end() && peek() != '"') {
        if (peek() == '\n') {
            m_line++;
        }
        advance();
    }

    if (is_at_end()) {
        return make_error_token("Unterminated string.");
    }

    // consume closing quote
    advance();
    return make_token(TokenType::TOKEN_STRING);
}

void Scanner::skip_whitespace() {
    while (true) {
        if (is_at_end()) {
            return;
        }
        char c = peek();
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n': {
            m_line++;
            advance();
        } break;
        case '/': {
            if (peek_next() == '/') {
                while (!is_at_end() && peek() != '\n') {
                    advance();
                }
            } else {
                return;
            }
        }
        default:
            return;
        }
    }
}

bool Scanner::is_at_end() {
    return m_current >= m_source.size();
}

char Scanner::advance() {
    return m_source.at(m_current++);
}

char Scanner::peek() {
    return m_source.at(m_current);
}

char Scanner::peek_next() {
    if (m_current + 1 >= m_source.size()) {
        return '\0';
    }
    return m_source.at(m_current + 1);
}

bool Scanner::match(char expected) {
    if (is_at_end()) {
        return false;
    }

    if (m_source.at(m_current) != expected) {
        return false;
    }

    m_current++;
    return true;
}

} // namespace scanner
