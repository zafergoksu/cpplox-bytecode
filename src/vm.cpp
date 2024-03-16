#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "utility.h"
#include "value.h"
#include <cstddef>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

using namespace chunk;
using namespace value;

namespace vm {

VirtualMachine::VirtualMachine(std::unique_ptr<chunk::Chunk> chunk)
    : m_chunk{std::move(chunk)},
      m_ip{0},
      m_table{} {
    m_stack.reserve(256);
}

void VirtualMachine::load_new_chunk(std::shared_ptr<chunk::Chunk> chunk) {
    m_chunk = std::move(chunk);
    m_ip = 0;
}

InterpretResult VirtualMachine::run() {
    InterpretResult result = INTERPRET_RUNTIME_ERROR;
    while (m_ip < m_chunk->size()) {
#ifdef DEBUG_TRACE_EXECUTION
        for (const auto& value : m_stack) {
            println("\t[ {} ]", value_to_string(value));
        }
        disassemble_instruction(*m_chunk, m_ip);
#endif
        result = run_step();
    }
    return result;
}

InterpretResult VirtualMachine::run_step() {
    u8 instruction = read_byte();
    switch (instruction) {
    case OP_CONSTANT: {
        Value constant = read_constant();
        push(constant);
        break;
    }
    case OP_NIL:
        push(nullptr);
        break;
    case OP_TRUE:
        push(true);
        break;
    case OP_FALSE:
        push(false);
        break;
    case OP_EQUAL: {
        Value rhs = pop();
        Value lhs = pop();
        const auto result = lhs == rhs;
        push(result);
        break;
    }
    case OP_GREATER:
        binary_greater_op();
        break;
    case OP_LESS:
        binary_less_op();
        break;
    case OP_ADD: {
        const Value& stack_top = peek_stack_top();
        const Value& stack_top_prev = peek(1);
        if (std::holds_alternative<ObjString>(stack_top) && std::holds_alternative<ObjString>(stack_top_prev)) {
            std::string new_string = std::get<ObjString>(stack_top_prev).str + std::get<ObjString>(stack_top).str;
            push(std::move(make_obj_string_interned(m_table, std::move(new_string))));
        } else if (std::holds_alternative<double>(stack_top) && std::holds_alternative<double>(stack_top_prev)) {
            binary_add_op();
        } else {
            runtime_error("Operands must be two numbers or two strings.");
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OP_SUBTRACT:
        binary_subtract_op();
        break;
    case OP_MULTIPLY:
        binary_multiply_op();
        break;
    case OP_DIVIDE:
        binary_divide_op();
        break;
    case OP_NOT:
        push(is_falsey(pop()));
        break;
    case OP_NEGATE: {
        if (!std::holds_alternative<double>(peek_stack_top())) {
            runtime_error("Operand must be a number.");
            return INTERPRET_RUNTIME_ERROR;
        }
        const auto negated_value = -std::get<double>(pop());
        push(negated_value);
        break;
    }
    case OP_RETURN: {
        println("{}", value_to_string(pop()));
        return INTERPRET_OK;
    }
    default:
        return INTERPRET_RUNTIME_ERROR;
    }
    return INTERPRET_RUNTIME_ERROR;
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
    m_stack.emplace_back(value);
}

const Value& VirtualMachine::peek_stack_top() const {
    return peek(0);
}

const Value& VirtualMachine::peek(usize n) const {
    return m_stack.at(m_stack.size() - 1 - n);
}

Value VirtualMachine::pop() {
    Value stack_top = m_stack.back();
    m_stack.pop_back();
    return stack_top;
}

void VirtualMachine::runtime_error(const std::string& message) {
    usize line = m_chunk->get_lines().at(m_ip);
    println_err("[line {}] in script", line);
}

bool VirtualMachine::is_falsey(Value value) {
    if (std::holds_alternative<std::nullptr_t>(value)) {
        return true;
    }

    if (std::holds_alternative<bool>(value)) {
        return !std::get<bool>(value);
    }

    return false;
}

inline InterpretResult VirtualMachine::pop_binary_operands(double& out_lhs, double& out_rhs) {
    const auto rhs = pop();
    const auto lhs = pop();
    if (!std::holds_alternative<double>(lhs) || !std::holds_alternative<double>(rhs)) {
        runtime_error("Operands must be numbers.");
        return INTERPRET_RUNTIME_ERROR;
    }

    out_lhs = std::get<double>(lhs);
    out_rhs = std::get<double>(rhs);
    return INTERPRET_OK;
}

inline InterpretResult VirtualMachine::binary_add_op() {
    double lhs = 0;
    double rhs = 0;
    InterpretResult result = pop_binary_operands(lhs, rhs);
    if (result != INTERPRET_OK) {
        return result;
    }
    push(lhs + rhs);
    return INTERPRET_OK;
}

inline InterpretResult VirtualMachine::binary_subtract_op() {
    double lhs = 0;
    double rhs = 0;
    InterpretResult result = pop_binary_operands(lhs, rhs);
    if (result != INTERPRET_OK) {
        return result;
    }
    push(lhs - rhs);
    return INTERPRET_OK;
}

inline InterpretResult VirtualMachine::binary_multiply_op() {
    double lhs = 0;
    double rhs = 0;
    InterpretResult result = pop_binary_operands(lhs, rhs);
    if (result != INTERPRET_OK) {
        return result;
    }
    push(lhs * rhs);
    return INTERPRET_OK;
}

inline InterpretResult VirtualMachine::binary_divide_op() {
    double lhs = 0;
    double rhs = 0;
    InterpretResult result = pop_binary_operands(lhs, rhs);
    if (result != INTERPRET_OK) {
        return result;
    }
    push(lhs / rhs);
    return INTERPRET_OK;
}

inline InterpretResult VirtualMachine::binary_greater_op() {
    double lhs = 0;
    double rhs = 0;
    InterpretResult result = pop_binary_operands(lhs, rhs);
    if (result != INTERPRET_OK) {
        return result;
    }
    push(lhs > rhs);
    return INTERPRET_OK;
}

inline InterpretResult VirtualMachine::binary_less_op() {
    double lhs = 0;
    double rhs = 0;
    InterpretResult result = pop_binary_operands(lhs, rhs);
    if (result != INTERPRET_OK) {
        return result;
    }
    push(lhs < rhs);
    return INTERPRET_OK;
}

} // namespace vm
