#include "scanner.h"
#include "token.h"
#include <string>
#include <unordered_map>
#include <utility>

using namespace token;

namespace scanner {

/*
 * We could use a trie for mapping keywords as we can disregard any keyword
 * searches that do not begin with a beginning character in this list.
 * However, std::unordered_map is fast enough for retrieving token types.
*/
static std::unordered_map<std::string, TokenType> token_mapping{
    {"and", TokenType::TOKEN_AND},
    {"class", TokenType::TOKEN_CLASS},
    {"else", TokenType::TOKEN_ELSE},
    {"if", TokenType::TOKEN_IF},
    {"nil", TokenType::TOKEN_NIL},
    {"or", TokenType::TOKEN_OR},
    {"print", TokenType::TOKEN_PRINT},
    {"return", TokenType::TOKEN_RETURN},
    {"super", TokenType::TOKEN_SUPER},
    {"var", TokenType::TOKEN_VAR},
    {"while", TokenType::TOKEN_WHILE},
    {"false", TokenType::TOKEN_FALSE},
    {"for", TokenType::TOKEN_FOR},
    {"fun", TokenType::TOKEN_FUN},
    {"this", TokenType::TOKEN_THIS},
    {"true", TokenType::TOKEN_TRUE},
};

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

    if (is_alpha(c)) {
        return identifier();
    }

    if (is_digit(c)) {
        return number();
    }

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

Token Scanner::number() {
    while (is_digit(peek())) {
        advance();
    }

    // Look for fractional part
    if (peek() == '.' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek())) {
            advance();
        }
    }

    return make_token(TokenType::TOKEN_NUMBER);
}

Token Scanner::identifier() {
    while (is_alpha(peek()) || is_digit(peek())) {
        advance();
    }

    std::string lexeme = m_source.substr(m_start, m_current - m_start);

    auto iter = token_mapping.find(lexeme);
    if (iter != token_mapping.end()) {
        return make_token(iter->second);
    }

    return make_token(TokenType::TOKEN_IDENTIFIER);
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
    if (is_at_end()) {
        return '\0';
    }
    return m_source.at(m_current);
}

char Scanner::peek_next() {
    if (m_current > m_source.size() - 2) {
        return '\0';
    }
    return m_source.at(m_current + 1);
}

bool Scanner::is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::is_digit(char c) {
    return c >= '0' && c <= '9';
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
