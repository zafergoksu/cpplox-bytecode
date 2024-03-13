#pragma once

#include "common.h"
#include "value.h"

#include <memory>
#include <utility>
#include <vector>

namespace chunk {
class Chunk;
}
namespace vm {

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VirtualMachine {
public:
    VirtualMachine() = default;
    explicit VirtualMachine(std::unique_ptr<chunk::Chunk> chunk);
    InterpretResult run();
    InterpretResult run_step();
    [[nodiscard]] usize get_ip() const;
    void load_new_chunk(std::shared_ptr<chunk::Chunk> chunk);
    const value::Value& peek_stack_top() const;
    const value::Value& peek(usize n) const;

private:
    u8 read_byte();
    value::Value read_constant();
    void push(value::Value value);
    value::Value pop();
    void runtime_error(const std::string& message);

    bool is_falsey(value::Value value);
    bool values_equal(value::Value lhs, value::Value rhs);
    inline InterpretResult pop_binary_operands(double& lhs, double& rhs);
    inline InterpretResult binary_add_op();
    inline InterpretResult binary_subtract_op();
    inline InterpretResult binary_multiply_op();
    inline InterpretResult binary_divide_op();
    inline InterpretResult binary_greater_op();
    inline InterpretResult binary_less_op();

    std::shared_ptr<const chunk::Chunk> m_chunk;
    usize m_ip;
    std::vector<value::Value> m_stack;
};

} // namespace vm
