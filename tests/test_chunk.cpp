#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <variant>

using ::testing::ContainerEq;

TEST(Chunk, test_write_byte) {
    chunk::Chunk chunk;
    chunk.write_byte(chunk::OpCode::OP_RETURN, 123);
    chunk.write_byte(chunk::OpCode::OP_RETURN, 321);
    std::vector<u8> code{
        chunk::OpCode::OP_RETURN,
        chunk::OpCode::OP_RETURN};
    std::vector<usize> lines{
        123,
        321};
    EXPECT_THAT(chunk.get_code(), ContainerEq(code));
    EXPECT_THAT(chunk.get_lines(), ContainerEq(lines));
}

TEST(Chunk, test_write_constant) {
    chunk::Chunk chunk;
    double constant_value = 1.23;
    auto value_to_write = std::make_shared<object::NumberObject>(constant_value);
    auto constant_idx = chunk.write_constant(value_to_write);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    EXPECT_EQ(constant_idx, 0);
    auto result = std::static_pointer_cast<object::NumberObject>(chunk.get_constants().get_values().at(constant_idx));
    EXPECT_EQ(result->value, constant_value);
}

int main(int ac, char* av[]) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
