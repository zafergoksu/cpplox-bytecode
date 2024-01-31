#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "utility.h"
#include "value.h"
#include <memory>
#include <vector>

using namespace chunk;
using namespace value;

namespace vm {

VirtualMachine::VirtualMachine(std::unique_ptr<chunk::Chunk> chunk)
    : m_chunk{std::move(chunk)},
      m_ip{0} {
    m_stack.reserve(256);
}

void VirtualMachine::load_new_chunk(std::unique_ptr<chunk::Chunk> chunk) {
    m_chunk = std::move(chunk);
    m_ip = 0;
}

InterpretResult VirtualMachine::run() {
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        for (const auto& value : m_stack) {
            println("\t[ {} ]", value);
        }
        disassemble_instruction(*m_chunk, m_ip);
#endif
        u8 instruction = read_byte();
        switch (instruction) {
        case OP_CONSTANT: {
            Value constant = read_constant();
            push(constant);
            break;
        }
        case OP_ADD:
            binary_add_op();
            break;
        case OP_SUBTRACT:
            binary_subtract_op();
            break;
        case OP_MULTIPLY:
            binary_multiply_op();
            break;
        case OP_DIVIDE:
            binary_divide_op();
            break;
        case OP_NEGATE:
            push(-pop());
            break;
        case OP_RETURN:
            println("{}", pop());
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

void VirtualMachine::push(Value value) {
    m_stack.emplace_back(std::move(value));
}

Value VirtualMachine::pop() {
    Value stack_top = m_stack.back();
    m_stack.pop_back();
    return stack_top;
}

inline void VirtualMachine::binary_add_op() {
    Value a = pop();
    Value b = pop();
    push(a + b);
}

inline void VirtualMachine::binary_subtract_op() {
    Value a = pop();
    Value b = pop();
    push(a - b);
}

inline void VirtualMachine::binary_multiply_op() {
    Value a = pop();
    Value b = pop();
    push(a * b);
}

inline void VirtualMachine::binary_divide_op() {
    Value a = pop();
    Value b = pop();
    push(a / b);
}

} // namespace vm
