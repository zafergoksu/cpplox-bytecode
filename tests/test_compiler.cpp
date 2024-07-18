#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "value.h"
#include "gtest/gtest.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace compiler;
using namespace chunk;
using namespace scanner;
using namespace value;

using testing::Eq;

// TODO(zafergoksu): expand tests to read from files

class CompilerTest : public testing::Test {
protected:
    void setup_compiler(std::string source) {
        m_scanner->load_source(std::move(source));
    }

    bool scanner_and_chunk_are_valid() {
        return m_scanner != nullptr && m_current_chunk != nullptr;
    }

    std::string read_file_to_string(const std::string& filename) {
        std::string alternative_path = "./test_files/" + filename;
        m_test_file_stream.open(filename);
        if (!m_test_file_stream.is_open()) {
            m_test_file_stream.open(alternative_path);
            if (!m_test_file_stream.is_open()) {
                throw std::runtime_error{"Could not open file: '" + filename + "'. Current dir: " + std::filesystem::current_path().string()};
            }
        }

        std::stringstream buffer;
        buffer << m_test_file_stream.rdbuf();
        return buffer.str();
    }

    std::shared_ptr<Scanner> m_scanner = std::make_shared<Scanner>();
    std::shared_ptr<Chunk> m_current_chunk = std::make_shared<Chunk>();
    Compiler m_compiler{m_scanner, m_current_chunk};
    std::ifstream m_test_file_stream;
};

TEST_F(CompilerTest, test_number_literals) {
    std::string filename = "number_literals.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }

    setup_compiler(source);

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
    std::string filename = "grouping.lox";

    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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
    std::string filename = "unary_negation.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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
    std::string filename = "arithmetic_ops.lox";
    std::string source;

    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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
    std::string filename = "boolean.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
}

TEST_F(CompilerTest, test_nil_value) {
    std::string filename = "nil_value.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
}

TEST_F(CompilerTest, test_not_op) {
    std::string filename = "not_op.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
}

TEST_F(CompilerTest, test_equality_op) {
    std::string filename = "equality_op.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
}

TEST_F(CompilerTest, test_string_expression) {
    std::string filename = "string_expression.lox";
    std::string source;
    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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
    std::string filename = "string_concatenation_op.lox";
    std::string source;

    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

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

TEST_F(CompilerTest, test_block_statment) {
    std::string filename = "block_statement.lox";
    std::string source;

    try {
        source = read_file_to_string(filename);
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
    setup_compiler(source);

    if (!scanner_and_chunk_are_valid()) {
        FAIL() << "Scanner and chunk are not valid pointers.";
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
        OpCode::OP_SET_LOCAL,
        0x00,
        OpCode::OP_POP,
        OpCode::OP_GET_LOCAL,
        0x00,
        OpCode::OP_PRINT,
        OpCode::OP_POP,
        OpCode::OP_RETURN};

    std::vector<value::Value> expect_constants{10.0, 20.0};

    EXPECT_EQ(result, true);
    EXPECT_EQ(out_size, expect_bytes.size());
    EXPECT_THAT(out_bytes, Eq(expect_bytes));
    EXPECT_THAT(out_constants.get_values(), Eq(expect_constants));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
