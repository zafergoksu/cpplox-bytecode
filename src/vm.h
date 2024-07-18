#pragma once

#include "common.h"
#include "table.h"
#include "value.h"

#include <array>
#include <memory>

namespace object {
class Object;
} // namespace object

namespace chunk {
class Chunk;
} // namespace chunk

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
    std::shared_ptr<object::Object> peek_stack_top() const;
    std::shared_ptr<object::Object> peek(usize n) const;
    void reset();

private:
    u8 read_byte();
    u16 read_short();
    std::shared_ptr<object::Object> read_constant();
    void push(std::shared_ptr<object::Object> value);
    std::shared_ptr<object::Object> pop();
    void runtime_error(const std::string& message);

    inline void concatenate();
    inline InterpretResult pop_binary_operands(double& lhs, double& rhs);
    inline InterpretResult binary_add_op();
    inline InterpretResult binary_subtract_op();
    inline InterpretResult binary_multiply_op();
    inline InterpretResult binary_divide_op();
    inline InterpretResult binary_greater_op();
    inline InterpretResult binary_less_op();

    std::shared_ptr<const chunk::Chunk> m_chunk;
    usize m_ip;
    table::Table m_strings;
    table::Table m_globals;
    u8 m_stack_top;
    std::array<std::shared_ptr<object::Object>, UINT8_COUNT> m_stack;
};

} // namespace vm
