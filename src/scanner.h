#pragma once

#include "common.h"
#include "token.h"

#include <string>

namespace scanner {

class Scanner {
public:
    Scanner();
    explicit Scanner(std::string source);

    void load_source(std::string source);
    token::Token scan_token();

private:
    bool is_at_end();
    char advance();
    char peek();
    char peek_next();
    bool match(char expected);
    void skip_whitespace();

    token::Token make_token(token::TokenType token_type);
    token::Token make_error_token(std::string error_message);

    token::Token string();

    std::string m_source;
    usize m_start;
    usize m_current;
    usize m_line;
};

} // namespace scanner
