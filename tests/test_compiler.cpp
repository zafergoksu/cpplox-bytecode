#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "value.h"
#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace compiler;
using namespace chunk;
using namespace scanner;
using namespace value;

using testing::Eq;

// TODO(zafergoksu): expand tests to read from files

class CompilerTest : public testing::Test {
protected:
    static std::string test_number_literals() {
        return "123.1;";
    }

    static std::string test_grouping() {
        return "5 * (123 + 1);";
    }

    static std::string test_unary_negation() {
        return "-123;";
    }

    static std::string test_arithmetic_ops() {
        return "1 + 4 * 3;";
    }

    static std::string test_boolean_values() {
        return "true;";
    }

    static std::string test_nil_value() {
        return "nil;";
    }

    static std::string test_not_op() {
        return "!true;";
    }

    static std::string test_equality_op() {
        return "true != false;";
    }

    static std::string test_string_expression() {
        return "\"Hello, world!\";";
    }

    static std::string test_string_concatenation_op() {
        return "\"Hello, world!\" + \" hi\";";
    }

    void setup_compiler(std::string source) {
        m_scanner->load_source(std::move(source));
    }

    bool scanner_and_chunk_are_valid() {
        return m_scanner != nullptr && m_current_chunk != nullptr;
    }

    std::shared_ptr<Scanner> m_scanner = std::make_shared<Scanner>();
    std::shared_ptr<Chunk> m_current_chunk = std::make_shared<Chunk>();
    Compiler m_compiler{m_scanner, m_current_chunk};
};

TEST_F(CompilerTest, test_number_literals) {
    setup_compiler(test_number_literals());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_CONSTANT,
        0x00,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{123.1};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_grouping) {
    setup_compiler(test_grouping());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_CONSTANT,
        0x00,
        OpCode::OP_CONSTANT,
        0x01,
        OpCode::OP_CONSTANT,
        0x02,
        OpCode::OP_ADD,
        OpCode::OP_MULTIPLY,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{5.0, 123.0, 1.0};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_unary_negation) {
    setup_compiler(test_unary_negation());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_CONSTANT,
        0x00,
        OpCode::OP_NEGATE,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{123.0};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_arithmetic_ops) {
    setup_compiler(test_arithmetic_ops());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_CONSTANT,
        0x00,
        OpCode::OP_CONSTANT,
        0x01,
        OpCode::OP_CONSTANT,
        0x02,
        OpCode::OP_MULTIPLY,
        OpCode::OP_ADD,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{1.0, 4.0, 3.0};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_boolean_values) {
    setup_compiler(test_boolean_values());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_TRUE,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_nil_value) {
    setup_compiler(test_nil_value());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_NIL,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_not_op) {
    setup_compiler(test_not_op());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_TRUE,
        OpCode::OP_NOT,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_equality_op) {
    setup_compiler(test_equality_op());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_TRUE,
        OpCode::OP_FALSE,
        OpCode::OP_EQUAL,
        OpCode::OP_NOT,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_string_expression) {
    setup_compiler(test_string_expression());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_CONSTANT,
        0x00,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    ObjString string_1 = make_obj_string("Hello, world!");
    std::vector<value::Value> expect_constants{string_1};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

TEST_F(CompilerTest, test_string_concatenation_op) {
    setup_compiler(test_string_concatenation_op());

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers";
    }

    bool result = m_compiler.compile();

    auto out_size = m_current_chunk->size();
    auto out_bytes = m_current_chunk->get_code();
    auto out_constants = m_current_chunk->get_constants();

    std::vector<u8> expect_bytes{
        OpCode::OP_CONSTANT,
        0x00,
        OpCode::OP_CONSTANT,
        0x01,
        OpCode::OP_ADD,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    ObjString string_1 = make_obj_string("Hello, world!");
    ObjString string_2 = make_obj_string(" hi");
    std::vector<value::Value> expect_constants{string_1, string_2};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
