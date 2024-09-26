#pragma once

#include "common.h"
#include "table.h"
#include "utility.h"
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
    explicit VirtualMachine(std::unique_ptr<chunk::Chunk> chunk);
    InterpretResult run();
    InterpretResult run_step();
    [[nodiscard]] usize get_ip() const;
    void load_new_chunk(std::shared_ptr<chunk::Chunk> chunk);
    std::shared_ptr<object::Object> peek_stack_top() const;
    std::shared_ptr<object::Object> peek(usize n) const;
    void reset();

private:
    u8 read_byte();
    u16 read_short();
    std::shared_ptr<object::Object> read_constant();
    void push(std::shared_ptr<object::Object> value);
    std::shared_ptr<object::Object> pop();
    void runtime_error(const std::string& message);

    inline void concatenate() {
        auto rhs = pop();
        auto lhs = pop();

        std::string new_string = lhs->to_string() + rhs->to_string();
        push(value::make_obj_string_interned(m_strings, std::move(new_string)));
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

        out_lhs = std::static_pointer_cast<object::NumberObject>(lhs)->value;
        out_rhs = std::static_pointer_cast<object::NumberObject>(rhs)->value;
        return INTERPRET_OK;
    }

    inline InterpretResult binary_add_op() {
        double lhs = 0;
        double rhs = 0;

        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }

        push(std::make_shared<object::NumberObject>(lhs + rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_subtract_op() {
        double lhs = 0;
        double rhs = 0;

        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }

        push(std::make_shared<object::NumberObject>(lhs - rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_multiply_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(std::make_shared<object::NumberObject>(lhs * rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_divide_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(std::make_shared<object::NumberObject>(lhs / rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_greater_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(std::make_shared<object::BooleanObject>(lhs > rhs));
        return INTERPRET_OK;
    }

    inline InterpretResult binary_less_op() {
        double lhs = 0;
        double rhs = 0;
        InterpretResult result = pop_binary_operands(lhs, rhs);
        if (result != INTERPRET_OK) {
            return result;
        }
        push(std::make_shared<object::BooleanObject>(lhs < rhs));
        return INTERPRET_OK;
    }

    std::shared_ptr<const chunk::Chunk> m_chunk;
    usize m_ip;
    table::Table m_strings;
    table::Table m_globals;
    u8 m_stack_top;
    std::array<std::shared_ptr<object::Object>, UINT8_COUNT> m_stack;
};

} // namespace vm
