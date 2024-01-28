#include "chunk.h"
#include "memory.h"
#include "value.h"
#include "vm.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
std::unique_ptr<chunk::Chunk> simple_constant_program() {
    auto chunk = std::make_unique<chunk::Chunk>();

    usize constant_idx = chunk->write_constant(1.2);
    chunk->write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    chunk->write_byte(chunk::OpCode::OP_RETURN, 123);
    return chunk;
}
} // namespace

TEST(VirtualMachine, test_simple_constant_program) {
    auto prog = simple_constant_program();
    usize prog_size = prog->get_code().size();
    vm::VirtualMachine vm{std::move(prog)};
    vm::InterpretResult result = vm.run();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
    EXPECT_EQ(vm.get_ip(), prog_size);
}

TEST(VirtualMachine, test_load_new_chunk) {
    auto prog_1 = simple_constant_program();
    auto prog_2 = simple_constant_program();
    usize prog_size_1 = prog_1->get_code().size();
    usize prog_size_2 = prog_2->get_code().size();
    ASSERT_NE(prog_1.get(), prog_2.get());

    vm::VirtualMachine vm{std::move(prog_1)};
    vm::InterpretResult result = vm.run();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
    auto prog_ip_1 = vm.get_ip();
    EXPECT_EQ(prog_ip_1, prog_size_1);

    vm.load_new_chunk(std::move(prog_2));
    result = vm.run();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
    auto prog_ip_2 = vm.get_ip();
    EXPECT_EQ(prog_ip_2, prog_size_2);
    EXPECT_EQ(prog_ip_1, prog_ip_2);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}