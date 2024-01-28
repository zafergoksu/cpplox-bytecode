#include "vm.h"
#include "chunk.h"
#include "common_types.h"
#include "utility.h"
#include <memory>
#include <vector>

using namespace chunk;
using namespace value;

namespace vm {

VirtualMachine::VirtualMachine(std::unique_ptr<chunk::Chunk> chunk)
    : m_chunk{std::move(chunk)},
      m_ip{0} {}

void VirtualMachine::load_new_chunk(std::unique_ptr<chunk::Chunk> chunk) {
    m_chunk = std::move(chunk);
    m_ip = 0;
}

InterpretResult VirtualMachine::run() {
    while (true) {
        u8 instruction = read_byte();
        switch (instruction) {
        case OP_CONSTANT: {
            Value constant = read_constant();
            println("{}", constant);
        } break;
        case OP_RETURN:
            return INTERPRET_OK;
        }
    }
}

usize VirtualMachine::get_ip() const {
    return m_ip;
}

u8 VirtualMachine::read_byte() {
    return m_chunk->get_code().at(m_ip++);
}

Value VirtualMachine::read_constant() {
    return m_chunk->get_constants().get_values().at(read_byte());
}

} // namespace vm