#pragma once

#include "chunk.h"
#include "common_types.h"
#include "value.h"
#include <memory>

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
    std::unique_ptr<chunk::Chunk> m_chunk;
    usize m_ip;
};

} // namespace vm
