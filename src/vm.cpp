#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "heap.h"
#include "object.h"
#include "utility.h"
#include "value.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace chunk;
using namespace value;
using namespace object;

using ds::Heap;

namespace vm {

VirtualMachine::VirtualMachine()
    : m_chunk{nullptr},
      m_heap{nullptr},
      m_ip{0},
      m_globals{},
      m_stack_top{0},
      m_stack{} {}

VirtualMachine::VirtualMachine(std::unique_ptr<chunk::Chunk> chunk, std::shared_ptr<Heap> heap)
    : m_chunk{std::move(chunk)},
      m_heap{std::move(heap)},
      m_ip{0},
      m_globals{},
      m_stack_top{0},
      m_stack{} {}

void VirtualMachine::reset() {
    m_chunk = nullptr;
    m_heap = nullptr;
    m_ip = 0;
    m_globals = {};
    m_stack_top = 0;
    m_stack = {};
}

void VirtualMachine::load_new_chunk(std::shared_ptr<chunk::Chunk> chunk, std::shared_ptr<Heap> heap) {
    m_chunk = std::move(chunk);
    m_heap = std::move(heap);
    m_ip = 0;
}

InterpretResult VirtualMachine::run() {
    InterpretResult result = INTERPRET_RUNTIME_ERROR;
    while (m_ip < m_chunk->size()) {
#ifdef DEBUG_TRACE_EXECUTION
        for (u8 i = 0; i < m_stack_top; i++) {
            println("\t[ {} ]", m_stack.at(i)->to_string());
        }
        disassemble_instruction(*m_chunk, m_ip);
#endif
        result = run_step();
        if (result != INTERPRET_OK) {
            return result;
        }
    }
    return result;
}

InterpretResult VirtualMachine::run_step() {
    u8 instruction = read_byte();
    switch (instruction) {
    case OpCode::OP_CONSTANT: {
        Object* constant = read_constant();
        push(constant);
        break;
    }
    case OpCode::OP_NIL:
        push(m_heap->make_object<NullObject>());
        break;
    case OpCode::OP_TRUE:
        push(m_heap->make_object<BooleanObject>(true));
        break;
    case OpCode::OP_FALSE:
        push(m_heap->make_object<BooleanObject>(false));
        break;
    case OpCode::OP_POP:
        pop();
        break;
    case OpCode::OP_GET_LOCAL: {
        u8 slot = read_byte();
        // get the index to the local variable slot (compiler and vm local var stacks match)
        // and push it back on top as we require other instructions to look at top of stack
        push(m_stack[slot]);
        break;
    }
    case OpCode::OP_SET_LOCAL: {
        u8 slot = read_byte();
        m_stack[slot] = peek_stack_top();
        break;
    }
    case OpCode::OP_GET_GLOBAL: {
        StringObject* name = static_cast<StringObject*>(read_constant());
        Object* value = m_globals.get(name);
        if (value == nullptr) {
            runtime_error("Undefined variable '" + name->to_string() + "'.");
            return INTERPRET_RUNTIME_ERROR;
        }
        push(value);
        break;
    }
    case OpCode::OP_DEFINE_GLOBAL: {
        auto name = static_cast<StringObject*>(read_constant());
        m_globals.set(name, peek_stack_top());
        pop();
        break;
    }
    case OpCode::OP_SET_GLOBAL: {
        auto name = static_cast<StringObject*>(read_constant());
        // when we set, we haven't defined it before
        if (m_globals.set(name, peek_stack_top())) {
            // delete old value for continuous use in repl
            m_globals.del(name);
            runtime_error("Undefined variable '" + name->to_string() + "'.");
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OpCode::OP_EQUAL: {
        Object* rhs = pop();
        Object* lhs = pop();
        bool result = lhs->is_equal(*rhs);
        push(m_heap->make_object<BooleanObject>(result));
        break;
    }
    case OpCode::OP_GREATER:
        binary_greater_op();
        break;
    case OpCode::OP_LESS:
        binary_less_op();
        break;
    case OpCode::OP_ADD: {
        Object* stack_top = peek_stack_top();
        Object* stack_top_prev = peek(1);
        if (stack_top == nullptr || stack_top_prev == nullptr) {
            runtime_error("Operands are nil.");
            return INTERPRET_RUNTIME_ERROR;
        }
        if (stack_top->type == ObjectType::OBJ_STRING && stack_top_prev->type == ObjectType::OBJ_STRING) {
            concatenate();
        } else if (stack_top->type == ObjectType::OBJ_NUMBER && stack_top_prev->type == ObjectType::OBJ_NUMBER) {
            binary_add_op();
        } else {
            runtime_error("Operands must be two numbers or two strings.");
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OpCode::OP_SUBTRACT:
        binary_subtract_op();
        break;
    case OpCode::OP_MULTIPLY:
        binary_multiply_op();
        break;
    case OpCode::OP_DIVIDE:
        binary_divide_op();
        break;
    case OpCode::OP_NOT:
        push(m_heap->make_object<BooleanObject>(pop()->is_falsey()));
        break;
    case OpCode::OP_NEGATE: {
        Object* stack_top = peek_stack_top();
        if (stack_top != nullptr && stack_top->type != ObjectType::OBJ_NUMBER) {
            runtime_error("Operand must be a number.");
            return INTERPRET_RUNTIME_ERROR;
        }
        auto value = static_cast<NumberObject*>(pop());
        push(m_heap->make_object<NumberObject>(-value->value));
        break;
    }
    case OpCode::OP_PRINT: {
        Object* value = pop();
        println("{}", value->to_string());
        break;
    }
    case OpCode::OP_JUMP: {
        u16 offset = read_short();
        m_ip += offset;
        break;
    }
    case OpCode::OP_JUMP_IF_FALSE: {
        u16 offset = read_short();
        if (peek_stack_top()->is_falsey()) {
            m_ip += offset;
        }
        break;
    }
    case OpCode::OP_LOOP: {
        u16 offset = read_short();
        m_ip -= offset;
        break;
    }
    case OpCode::OP_RETURN: {
        // Exit virtual machine
        return INTERPRET_OK;
    }
    default:
        return INTERPRET_RUNTIME_ERROR;
    }
    return INTERPRET_OK;
}

usize VirtualMachine::get_ip() const {
    return m_ip;
}

u8 VirtualMachine::read_byte() {
    return m_chunk->get_code().at(m_ip++);
}

u16 VirtualMachine::read_short() {
    m_ip += 2;
    return (m_chunk->get_code().at(m_ip - 2) << 8) | (m_chunk->get_code().at(m_ip - 1));
}

Object* VirtualMachine::read_constant() {
    return m_chunk->get_constants().get_values().at(read_byte());
}

void VirtualMachine::push(Object* value) {
    m_stack[m_stack_top] = value;
    m_stack_top++;
}

Object* VirtualMachine::peek_stack_top() const {
    return peek(0);
}

Object* VirtualMachine::peek(usize n) const {
    return m_stack[m_stack_top - 1 - n];
}

Object* VirtualMachine::pop() {
    m_stack_top--;
    return m_stack[m_stack_top];
}

void VirtualMachine::runtime_error(const std::string& message) {
    print_err("{}", message);
    usize line = m_chunk->get_lines().at(m_ip);
    println_err("[line {}] in script", line);
}

} // namespace vm
