#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <variant>

using namespace chunk;
using namespace value;
using namespace object;

class VirtualMachineTest : public ::testing::Test {
protected:
    ~VirtualMachineTest() override = default;

    void TearDown() override {
        m_vm.reset();
    }

    static std::unique_ptr<Chunk> simple_constant_program() {
        auto chunk = std::make_unique<Chunk>();

        usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(1.2));
        chunk->write_byte(OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        chunk->write_byte(OpCode::OP_RETURN, 123);
        return chunk;
    }

    static std::unique_ptr<Chunk> binary_op_program(std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs, OpCode op_code) {
        auto chunk = std::make_unique<Chunk>();

        usize constant_idx = chunk->write_constant(std::move(lhs));
        chunk->write_byte(OpCode::OP_CONSTANT, 123);
        chunk->write_byte(constant_idx, 123);

        constant_idx = chunk->write_constant(std::move(rhs));
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

    usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(34.0));
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

    usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(2.0));
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    chunk->write_byte(OpCode::OP_NEGATE, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    m_vm.run_step();
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, -2.0);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_add) {
    auto prog = binary_op_program(std::make_shared<NumberObject>(1.2), std::make_shared<NumberObject>(3.4), OpCode::OP_ADD);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 4.6);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_subtract) {
    auto prog = binary_op_program(std::make_shared<NumberObject>(1.2), std::make_shared<NumberObject>(-3.4), OpCode::OP_SUBTRACT);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 4.6);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_multiply) {
    auto prog = binary_op_program(std::make_shared<NumberObject>(1.5), std::make_shared<NumberObject>(3.0), OpCode::OP_MULTIPLY);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 4.5);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_divide) {
    auto prog = binary_op_program(std::make_shared<NumberObject>(15.0), std::make_shared<NumberObject>(3.0), OpCode::OP_DIVIDE);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 5.0);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_op_failure) {
    auto prog = binary_op_program(nullptr, std::make_shared<NumberObject>(3.0), OpCode::OP_DIVIDE);
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

    auto stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, true);
}

TEST_F(VirtualMachineTest, test_false_bool_op) {
    auto chunk = std::make_unique<Chunk>();

    chunk->write_byte(OpCode::OP_FALSE, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, false);
}

TEST_F(VirtualMachineTest, test_not_op) {
    auto chunk = std::make_unique<Chunk>();

    chunk->write_byte(OpCode::OP_NIL, 123);

    chunk->write_byte(OpCode::OP_NOT, 123);
    chunk->write_byte(OpCode::OP_RETURN, 123);
    m_vm.load_new_chunk(std::move(chunk));

    m_vm.run_step();
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, true);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_equal_op) {
    auto prog = binary_op_program(std::make_shared<NumberObject>(15.0), std::make_shared<NumberObject>(15.0), OpCode::OP_EQUAL);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, true);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);

    prog = binary_op_program(std::make_shared<NumberObject>(15.0), std::make_shared<NumberObject>(3.0), OpCode::OP_EQUAL);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    result = m_vm.run_step();

    stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, false);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_binary_comparison_op) {
    auto prog = binary_op_program(std::make_shared<NumberObject>(15.0), std::make_shared<NumberObject>(13.0), OpCode::OP_GREATER);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, true);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);

    prog = binary_op_program(std::make_shared<NumberObject>(15.0), std::make_shared<NumberObject>(3.0), OpCode::OP_LESS);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    result = m_vm.run_step();

    stack_top = std::static_pointer_cast<BooleanObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, false);
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_string_concatenation) {
    auto string_1 = std::make_shared<StringObject>("Hello, ");
    auto string_2 = std::make_shared<StringObject>("world!");
    auto prog = binary_op_program(string_1, string_2, OpCode::OP_ADD);
    m_vm.load_new_chunk(std::move(prog));
    run_n_steps(2);
    auto result = m_vm.run_step();

    auto stack_top = std::static_pointer_cast<StringObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, "Hello, world!");
    result = m_vm.run_step();
    EXPECT_EQ(result, vm::InterpretResult::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_get_global_var) {
    auto string_1 = std::make_shared<StringObject>("a");

    auto chunk = std::make_unique<Chunk>();

    // testing var a = 1 + 2;
    usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(1.0));
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    constant_idx = chunk->write_constant(std::make_shared<NumberObject>(2.0));
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

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 3.0);
    auto result = m_vm.run_step();
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_set_global_var) {
    auto string_1 = std::make_shared<StringObject>("a");
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(1.0));
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);

    constant_idx = chunk->write_constant(std::make_shared<NumberObject>(2.0));
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

    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 3.0);
    auto result = m_vm.run_step();
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_get_local_var) {
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(10.0));
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);
    chunk->write_byte(OpCode::OP_GET_LOCAL, 123);
    chunk->write_byte(0, 123);
    chunk->write_byte(OpCode::OP_POP, 123);

    m_vm.load_new_chunk(std::move(chunk));
    run_n_steps(2);
    auto result = m_vm.run_step();
    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 10.0);
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

TEST_F(VirtualMachineTest, test_set_local_var) {
    auto chunk = std::make_unique<Chunk>();

    usize constant_idx = chunk->write_constant(std::make_shared<NumberObject>(10.0));
    chunk->write_byte(OpCode::OP_CONSTANT, 123);
    chunk->write_byte(constant_idx, 123);
    constant_idx = chunk->write_constant(std::make_shared<NumberObject>(20.0));
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
    auto stack_top = std::static_pointer_cast<NumberObject>(m_vm.peek_stack_top());
    EXPECT_EQ(stack_top->value, 20.0);
    EXPECT_EQ(result, vm::INTERPRET_OK);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
