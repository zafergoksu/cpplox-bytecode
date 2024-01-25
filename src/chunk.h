#ifndef CPPLOX_BYTECODE_SRC_CHUNK_H
#define CPPLOX_BYTECODE_SRC_CHUNK_H

#include "common_types.h"
#include "value.h"
#include <cstdint>
#include <vector>

namespace chunk {
enum OpCode : u8 {
    OP_CONSTANT,
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

#endif //CPPLOX_BYTECODE_SRC_CHUNK_H
