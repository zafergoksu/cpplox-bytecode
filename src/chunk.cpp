#include "chunk.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include <memory>
#include <vector>

using namespace object;

namespace chunk {
usize Chunk::size() const {
    return m_code.size();
}

void Chunk::write_byte(u8 byte, usize line) {
    m_code.emplace_back(byte);
    m_lines.emplace_back(line);
}

void Chunk::write_byte_at(usize offset, u8 byte) {
    m_code[offset] = byte;
}

const std::vector<u8>& Chunk::get_code() const {
    return m_code;
}

usize Chunk::write_constant(std::shared_ptr<Object> value) {
    m_constants.write_value(value);
    return m_constants.get_values().size() - 1;
}

const value::ValueArray& Chunk::get_constants() const {
    return m_constants;
}

const std::vector<usize>& Chunk::get_lines() const {
    return m_lines;
}

void Chunk::clear_constants() {
    m_constants.clear();
}
} // namespace chunk
