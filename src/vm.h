#pragma once

#include "chunk.h"
#include "common.h"
#include "value.h"
#include <memory>
#include <vector>

namespace vm {

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VirtualMachine {
public:
    VirtualMachine(std::unique_ptr<chunk::Chunk> chunk);
    InterpretResult run();
    usize get_ip() const;
    void load_new_chunk(std::unique_ptr<chunk::Chunk> chunk);

private:
    u8 read_byte();
    value::Value read_constant();
    void push(value::Value value);
    value::Value pop();
    inline void binary_add_op();
    inline void binary_subtract_op();
    inline void binary_multiply_op();
    inline void binary_divide_op();

    std::unique_ptr<chunk::Chunk> m_chunk;
    usize m_ip;
    std::vector<value::Value> m_stack;
};

} // namespace vm
