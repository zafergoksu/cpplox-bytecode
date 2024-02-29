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

    std::string lexeme = m_source.substr(m_start, m_current - m_start);
    switch (c) {
    case '(':
        return Token{TokenType::TOKEN_LEFT_PAREN, std::move(lexeme), m_line};
    case ')':
        return Token{TokenType::TOKEN_RIGHT_PAREN, std::move(lexeme), m_line};
    case '{':
        return Token{TokenType::TOKEN_LEFT_BRACE, std::move(lexeme), m_line};
    case '}':
        return Token{TokenType::TOKEN_RIGHT_BRACE, std::move(lexeme), m_line};
    case ';':
        return Token{TokenType::TOKEN_SEMICOLON, std::move(lexeme), m_line};
    case ',':
        return Token{TokenType::TOKEN_COMMA, std::move(lexeme), m_line};
    case '.':
        return Token{TokenType::TOKEN_DOT, std::move(lexeme), m_line};
    case '-':
        return Token{TokenType::TOKEN_MINUS, std::move(lexeme), m_line};
    case '+':
        return Token{TokenType::TOKEN_PLUS, std::move(lexeme), m_line};
    case '/':
        return Token{TokenType::TOKEN_SLASH, std::move(lexeme), m_line};
    case '*':
        return Token{TokenType::TOKEN_STAR, std::move(lexeme), m_line};
    case '!':
        return Token{match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG, m_source.substr(m_start, m_current - m_start), m_line};
    case '=':
        return Token{match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL, m_source.substr(m_start, m_current - m_start), m_line};
    case '<':
        return Token{match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS, m_source.substr(m_start, m_current - m_start), m_line};
    case '>':
        return Token{match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER, m_source.substr(m_start, m_current - m_start), m_line};
    }

    return Token{TokenType::TOKEN_ERROR, "Unexpected character.", m_line};
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
