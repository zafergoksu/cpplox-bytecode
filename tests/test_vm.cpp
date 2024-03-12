#include "chunk.h"
#include "vm.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <variant>

class VirtualMachineTest : public ::testing::Test {
protected:
    ~VirtualMachineTest() override = default;

    static std::unique_ptr<chunk::Chunk> simple_constant_program() {
        auto chunk = std::make_unique<chunk::Chunk>();

        usize constant_idx = chunk->write_constant(1.2);
        chunk->write_byte(chunk::OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        chunk->write_byte(chunk::OpCode::OP_RETURN, 123);
        return chunk;
    }

    static std::unique_ptr<chunk::Chunk> binary_op_program(value::Value lhs, value::Value rhs, chunk::OpCode op_code) {
        auto chunk = std::make_unique<chunk::Chunk>();

        usize constant_idx = chunk->write_constant(lhs);
        chunk->write_byte(chunk::OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        constant_idx = chunk->write_constant(rhs);
        chunk->write_byte(chunk::OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        chunk->write_byte(op_code, 123);
        chunk->write_byte(chunk::OpCode::OP_RETURN, 123);
        return chunk;
    }

    vm::VirtualMachine m_vm;
};

TEST_F(VirtualMachineTest, test_run_step) {
    auto chunk = std::make_unique<chunk::Chunk>();

    usize constant_idx = chunk->write_constant(34.0);
    chunk->write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    m_vm.load_new_chunk(std::move(chunk));
    vm::InterpretResult result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
}

TEST_F(VirtualMachineTest, test_run) {
    auto prog = simple_constant_program();
    usize prog_size = prog->get_code().size();
    m_vm.load_new_chunk(std::move(prog));
    vm::InterpretResult result = m_vm.run();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
    EXPECT_EQ(m_vm.get_ip(), prog_size);
}

TEST_F(VirtualMachineTest, test_load_new_chunk) {
    auto prog_1 = simple_constant_program();
    auto prog_2 = simple_constant_program();
    usize prog_size_1 = prog_1->get_code().size();
    usize prog_size_2 = prog_2->get_code().size();
    ASSERT_NE(prog_1.get(), prog_2.get());

    m_vm.load_new_chunk(std::move(prog_1));
    vm::InterpretResult result = m_vm.run();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
    auto prog_ip_1 = m_vm.get_ip();
    EXPECT_EQ(prog_ip_1, prog_size_1);

    m_vm.load_new_chunk(std::move(prog_2));
    result = m_vm.run();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
    auto prog_ip_2 = m_vm.get_ip();
    EXPECT_EQ(prog_ip_2, prog_size_2);
    EXPECT_EQ(prog_ip_1, prog_ip_2);
}

TEST_F(VirtualMachineTest, test_unary_op_negate) {
    auto chunk = std::make_unique<chunk::Chunk>();

    usize constant_idx = chunk->write_constant(2.0);
    chunk->write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    chunk->write_byte(chunk::OpCode::OP_NEGATE, 123);
    chunk->write_byte(chunk::OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), -2.0);
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_add) {
    auto prog = binary_op_program(1.2, 3.4, chunk::OpCode::OP_ADD);
    m_vm.load_new_chunk(std::move(prog));
    m_vm.run_step();
    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 4.6);
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_subtract) {
    auto prog = binary_op_program(1.2, -3.4, chunk::OpCode::OP_SUBTRACT);
    m_vm.load_new_chunk(std::move(prog));
    m_vm.run_step();
    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 4.6);
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_multiply) {
    auto prog = binary_op_program(1.5, 3.0, chunk::OpCode::OP_MULTIPLY);
    m_vm.load_new_chunk(std::move(prog));
    m_vm.run_step();
    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 4.5);
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_divide) {
    auto prog = binary_op_program(15.0, 3.0, chunk::OpCode::OP_DIVIDE);
    m_vm.load_new_chunk(std::move(prog));
    m_vm.run_step();
    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 5.0);
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
