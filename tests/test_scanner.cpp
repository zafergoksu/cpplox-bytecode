#include "scanner.h"
#include "token.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace token;
using namespace scanner;

class ScannerTest : public ::testing::Test {
protected:
    static std::string single_character_operators() {
        return "( ) { } ; , . - + / * ! = < >";
    }

    static std::string two_character_operators() {
        return "!= == <= >=";
    }

    static std::string test_comment() {
        return "() // hi this is a comment";
    }

    static std::string test_number_without_fractional() {
        return "123";
    }

    static std::string test_number_with_point_no_fractional() {
        return "123.";
    }

    static std::string test_double_number() {
        return "123.321";
    }

    static std::string test_special_keywords() {
        return "and class else false for fun if nil or print return super this true var while";
    }

    std::optional<std::vector<Token>> scan_tokens(usize expect_output_size) {
        std::vector<Token> output_tokens;
        while (true) {
            Token token = m_scanner.scan_token();
            output_tokens.emplace_back(token);

            if (output_tokens.back().get_type() == TokenType::TOKEN_EOF) {
                break;
            }

            if (output_tokens.size() > expect_output_size) {
                return std::nullopt;
            }
        }

        return output_tokens;
    }

    Scanner m_scanner;
};

TEST_F(ScannerTest, test_single_character_operator_scanning) {
    m_scanner.load_source(single_character_operators());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_LEFT_PAREN, "(", 1},
        {TokenType::TOKEN_RIGHT_PAREN, ")", 1},
        {TokenType::TOKEN_LEFT_BRACE, "{", 1},
        {TokenType::TOKEN_RIGHT_BRACE, "}", 1},
        {TokenType::TOKEN_SEMICOLON, ";", 1},
        {TokenType::TOKEN_COMMA, ",", 1},
        {TokenType::TOKEN_DOT, ".", 1},
        {TokenType::TOKEN_MINUS, "-", 1},
        {TokenType::TOKEN_PLUS, "+", 1},
        {TokenType::TOKEN_SLASH, "/", 1},
        {TokenType::TOKEN_STAR, "*", 1},
        {TokenType::TOKEN_BANG, "!", 1},
        {TokenType::TOKEN_EQUAL, "=", 1},
        {TokenType::TOKEN_LESS, "<", 1},
        {TokenType::TOKEN_GREATER, ">", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

TEST_F(ScannerTest, test_two_character_operators) {
    m_scanner.load_source(two_character_operators());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_BANG_EQUAL, "!=", 1},
        {TokenType::TOKEN_EQUAL_EQUAL, "==", 1},
        {TokenType::TOKEN_LESS_EQUAL, "<=", 1},
        {TokenType::TOKEN_GREATER_EQUAL, ">=", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

TEST_F(ScannerTest, test_comment) {
    m_scanner.load_source(test_comment());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_LEFT_PAREN, "(", 1},
        {TokenType::TOKEN_RIGHT_PAREN, ")", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

TEST_F(ScannerTest, test_number_without_fractional) {
    m_scanner.load_source(test_number_without_fractional());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_NUMBER, "123", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

TEST_F(ScannerTest, test_number_with_point_no_fractional) {
    m_scanner.load_source(test_number_with_point_no_fractional());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_NUMBER, "123", 1},
        {TokenType::TOKEN_DOT, ".", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

TEST_F(ScannerTest, test_double_number) {
    m_scanner.load_source(test_double_number());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_NUMBER, "123.321", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

TEST_F(ScannerTest, test_special_keywords) {
    m_scanner.load_source(test_special_keywords());

    std::vector<Token> expected_tokens{
        {TokenType::TOKEN_AND, "and", 1},
        {TokenType::TOKEN_CLASS, "class", 1},
        {TokenType::TOKEN_ELSE, "else", 1},
        {TokenType::TOKEN_FALSE, "false", 1},
        {TokenType::TOKEN_FOR, "for", 1},
        {TokenType::TOKEN_FUN, "fun", 1},
        {TokenType::TOKEN_IF, "if", 1},
        {TokenType::TOKEN_NIL, "nil", 1},
        {TokenType::TOKEN_OR, "or", 1},
        {TokenType::TOKEN_PRINT, "print", 1},
        {TokenType::TOKEN_RETURN, "return", 1},
        {TokenType::TOKEN_SUPER, "super", 1},
        {TokenType::TOKEN_THIS, "this", 1},
        {TokenType::TOKEN_TRUE, "true", 1},
        {TokenType::TOKEN_VAR, "var", 1},
        {TokenType::TOKEN_WHILE, "while", 1},
        {TokenType::TOKEN_EOF, "", 1}};

    std::optional<std::vector<Token>> output_tokens = scan_tokens(expected_tokens.size());

    if (!output_tokens) {
        FAIL() << "Output token size exceeded expect output token size.";
    }

    EXPECT_EQ(output_tokens.value().size(), expected_tokens.size());
    EXPECT_THAT(output_tokens.value(), testing::ContainerEq(expected_tokens));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
