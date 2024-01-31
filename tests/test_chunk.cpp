#include "chunk.h"
#include "common_types.h"
#include "value.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
    auto constant_idx = chunk.write_constant(constant_value);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    EXPECT_EQ(constant_idx, 0);
    EXPECT_EQ(chunk.get_constants().get_values().at(constant_idx), constant_value);
}

int main(int ac, char* av[]) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
