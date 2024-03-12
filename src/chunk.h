#pragma once

#include "common.h"
#include "value.h"
#include <vector>

namespace chunk {
enum OpCode : u8 {
    OP_CONSTANT,
    // dedicated op codes for nil, and boolean values
    // to push on stack rather than emit constants
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN
};

class Chunk {
public:
    [[nodiscard]] usize size() const;
    void write_byte(u8 byte, usize line);
    [[nodiscard]] usize write_constant(value::Value value);

    [[nodiscard]] const std::vector<u8>& get_code() const;
    [[nodiscard]] const std::vector<usize>& get_lines() const;
    [[nodiscard]] const value::ValueArray& get_constants() const;

private:
    std::vector<u8> m_code;
    std::vector<usize> m_lines;
    value::ValueArray m_constants;
};
} // namespace chunk
