#pragma once

#include "common.h"
#include "heap.h"
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
    VirtualMachine();
    VirtualMachine(std::unique_ptr<chunk::Chunk> chunk, std::shared_ptr<ds::Heap> heap);
    InterpretResult run();
    InterpretResult run_step();
    [[nodiscard]] usize get_ip() const;
    // TODO(zgoksu): consider ownership
    void load_new_chunk(std::shared_ptr<chunk::Chunk> chunk, std::shared_ptr<ds::Heap> heap);
    [[nodiscard]] object::Object* peek_stack_top() const;
    [[nodiscard]] object::Object* peek(usize n) const;
    void reset();

private:
    u8 read_byte();
    u16 read_short();
    object::Object* read_constant();
    void push(object::Object* value);
    object::Object* pop();
    void runtime_error(const std::string& message);

    inline void concatenate() {
        const auto rhs = pop();
        const auto lhs = pop();

        const std::string new_string = lhs->to_string() + rhs->to_string();
        push(m_heap->make_obj_string(new_string));
    }

    inline InterpretResult pop_binary_operands(double& out_lhs, double& out_rhs) {
        const auto rhs = pop();
        const auto lhs = pop();

        if (lhs == nullptr || rhs == nullptr) {
            return INTERPRET_RUNTIME_ERROR;
        }

        if (lhs->type != object::ObjectType::OBJ_NUMBER || rhs->type != object::ObjectType::OBJ_NUMBER) {
            runtime_error("Operands must be numbers.");
            return INTERPRET_RUNTIME_ERROR;

        }

        const auto lhs_ptr = dynamic_cast<object::NumberObject*>(lhs);
        const auto rhs_ptr = dynamic_cast<object::NumberObject*>(rhs);
        if (lhs_ptr == nullptr || rhs_ptr == nullptr) {
            runtime_error("Operands must be numbers.");
            return INTERPRET_RUNTIME_ERROR;
        }
        out_lhs = lhs_ptr->value;
        out_rhs = rhs_ptr->value;
        return INTERPRET_OK;
    }

    inline InterpretResult binary_add_op() {
        double lhs = 0;
        double rhs = 0;

        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }

        push(m_heap->make_object<object::NumberObject>(lhs + rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_subtract_op() {
        double lhs = 0;
        double rhs = 0;

        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }

        push(m_heap->make_object<object::NumberObject>(lhs - rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_multiply_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(m_heap->make_object<object::NumberObject>(lhs * rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_divide_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(m_heap->make_object<object::NumberObject>(lhs / rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_greater_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(m_heap->boolean(lhs > rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_less_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(m_heap->boolean(lhs < rhs));
        return INTERPRET_OK;
    }

    std::shared_ptr<const chunk::Chunk> m_chunk;
    std::shared_ptr<ds::Heap> m_heap;
    usize m_ip;
    table::Table m_globals;
    u8 m_stack_top;
    std::array<object::Object*, UINT8_COUNT> m_stack;
};

} // namespace vm
