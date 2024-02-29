#include "token.h"
#include "common.h"

#include <string>

namespace token {
Token::Token(token::TokenType type, std::string lexeme, usize line)
    : m_type{type},
      m_lexeme{std::move(lexeme)},
      m_line{line} {}

TokenType Token::get_type() const {
    return m_type;
}

const std::string& Token::get_lexeme() const {
    return m_lexeme;
}

usize Token::get_line() const {
    return m_line;
}

bool Token::operator==(const Token& other) const {
    return m_type == other.m_type && m_lexeme == other.m_lexeme && m_line == other.m_line;
}
} // namespace token