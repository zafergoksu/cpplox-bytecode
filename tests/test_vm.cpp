#include "chunk.h"
#include "common.h"
#include "value.h"
#include "vm.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <variant>

using namespace chunk;
using namespace value;

class VirtualMachineTest : public ::testing::Test {
protected:
    ~VirtualMachineTest() override = default;

    static std::unique_ptr<Chunk> simple_constant_program() {
        auto chunk = std::make_unique<Chunk>();

        usize constant_idx = chunk->write_constant(1.2);
        chunk->write_byte(OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        chunk->write_byte(OpCode::OP_RETURN, 123);
        return chunk;
    }

    static std::unique_ptr<Chunk> binary_op_program(Value lhs, Value rhs, OpCode op_code) {
        auto chunk = std::make_unique<Chunk>();

        usize constant_idx = chunk->write_constant(lhs);
        chunk->write_byte(OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        constant_idx = chunk->write_constant(rhs);
        chunk->write_byte(OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        chunk->write_byte(op_code, 123);
        chunk->write_byte(OpCode::OP_RETURN, 123);
        return chunk;
    }

    void run_n_steps(usize n) {
        for (usize i = 0; i < n; i++) {
            m_vm.run_step();
        }
    }

    vm::VirtualMachine m_vm;
};

TEST_F(VirtualMachineTest, test_run_step) {
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(34.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    m_vm.load_new_chunk(std::move(chunk));
    vm::InterpretResult result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
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
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(2.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    chunk->write_byte(OpCode::OP_NEGATE, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), -2.0);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_add) {
    auto prog = binary_op_program(1.2, 3.4, OpCode::OP_ADD);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 4.6);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_subtract) {
    auto prog = binary_op_program(1.2, -3.4, OpCode::OP_SUBTRACT);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 4.6);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_multiply) {
    auto prog = binary_op_program(1.5, 3.0, OpCode::OP_MULTIPLY);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 4.5);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_divide) {
    auto prog = binary_op_program(15.0, 3.0, OpCode::OP_DIVIDE);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 5.0);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_failure) {
    auto prog = binary_op_program(nullptr, 3.0, OpCode::OP_DIVIDE);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_op_negate_failure) {
    auto chunk = std::make_unique<Chunk>();

    chunk->write_byte(OpCode::OP_NIL, 123);

    chunk->write_byte(OpCode::OP_NEGATE, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_RUNTIME_ERROR);
}

TEST_F(VirtualMachineTest, test_true_bool_op) {
    auto chunk = std::make_unique<Chunk>();

    chunk->write_byte(OpCode::OP_TRUE, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), true);
}

TEST_F(VirtualMachineTest, test_false_bool_op) {
    auto chunk = std::make_unique<Chunk>();

    chunk->write_byte(OpCode::OP_FALSE, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), false);
}

TEST_F(VirtualMachineTest, test_not_op) {
    auto chunk = std::make_unique<Chunk>();

    chunk->write_byte(OpCode::OP_NIL, 123);

    chunk->write_byte(OpCode::OP_NOT, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    m_vm.run_step();
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), true);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_equal_op) {
    auto prog = binary_op_program(15.0, 15.0, OpCode::OP_EQUAL);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), true);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);

    prog = binary_op_program(15.0, 3.0, OpCode::OP_EQUAL);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), false);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_comparison_op) {
    auto prog = binary_op_program(15.0, 13.0, OpCode::OP_GREATER);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), true);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);

    prog = binary_op_program(15.0, 3.0, OpCode::OP_LESS);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    result = m_vm.run_step();

    EXPECT_EQ(std::get<bool>(m_vm.peek_stack_top()), false);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_string_concatenation) {
    ObjString string_1 = make_obj_string("Hello, ");
    ObjString string_2 = make_obj_string("world!");
    auto prog = binary_op_program(string_1, string_2, OpCode::OP_ADD);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    EXPECT_EQ(std::get<ObjString>(m_vm.peek_stack_top()).str, "Hello, world!");
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_get_global_var) {
    ObjString string_1 = make_obj_string("a");
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(1.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    constant_idx = chunk->write_constant(2.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    chunk->write_byte(OpCode::OP_ADD, 123);
    constant_idx = chunk->write_constant(string_1);
    chunk->write_byte(OpCode::OP_DEFINE_GLOBAL, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_GET_GLOBAL, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);

    m_vm.load_new_chunk(std::move(chunk));
    run_n_steps(5);

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 3.0);
    auto result = m_vm.run_step();
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_set_global_var) {
    ObjString string_1 = make_obj_string("a");
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(1.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    constant_idx = chunk->write_constant(2.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    chunk->write_byte(OpCode::OP_ADD, 123);
    constant_idx = chunk->write_constant(string_1);
    chunk->write_byte(OpCode::OP_DEFINE_GLOBAL, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_GET_GLOBAL, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);

    m_vm.load_new_chunk(std::move(chunk));
    run_n_steps(5);

    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 3.0);
    auto result = m_vm.run_step();
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_get_local_var) {
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(10.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_GET_LOCAL, 123);
    chunk->write_byte(0, 123);
    chunk->write_byte(OpCode::OP_POP, 123);

    m_vm.load_new_chunk(std::move(chunk));
    run_n_steps(2);
    auto result = m_vm.run_step();
    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 10.0);
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_set_local_var) {
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(10.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);
    constant_idx = chunk->write_constant(20.0);
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_SET_LOCAL, 123);
    chunk->write_byte(0, 123);
    chunk->write_byte(OpCode::OP_POP, 123);
    chunk->write_byte(OpCode::OP_GET_LOCAL, 123);
    chunk->write_byte(0, 123);

    m_vm.load_new_chunk(std::move(chunk));
    run_n_steps(4);
    auto result = m_vm.run_step();
    EXPECT_EQ(std::get<double>(m_vm.peek_stack_top()), 20.0);
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
