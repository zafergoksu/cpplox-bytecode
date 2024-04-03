#include "debug.h"
#include "chunk.h"
#include "common.h"
#include "utility.h"
#include "value.h"
#include <cstddef>
#include <string>
#include <variant>

using namespace chunk;

namespace {
usize simple_instruction(const std::string& name, usize offset) {
    println("{}", name);
    return offset + 1;
}

usize constant_instruction(const std::string& name, const Chunk& chunk, usize offset) {
    u8 constant = chunk.get_code().at(offset + 1);
    print("{:16s} {:4d} '", name, constant);

    auto value = chunk.get_constants().get_values().at(constant);
    std::visit(value::print_visitor{}, value);
    println("'");

    return offset + 2;
}
} // namespace

usize disassemble_instruction(const Chunk& chunk, usize offset) {
    print("{:04d} ", offset);
    if (offset > 0 && chunk.get_lines().at(offset) == chunk.get_lines().at(offset - 1)) {
        print("\t| ");
    } else {
        print("{:4d} ", chunk.get_lines().at(offset));
    }

    u8 instruction = chunk.get_code().at(offset);
    switch (instruction) {
    case OpCode::OP_RETURN:
        return simple_instruction("OP_RETURN", offset);
    case OpCode::OP_PRINT:
        return simple_instruction("OP_PRINT", offset);
    case OpCode::OP_NEGATE:
        return simple_instruction("OP_NEGATE", offset);
    case OpCode::OP_CONSTANT:
        return constant_instruction("OP_CONSTANT", chunk, offset);
    case OpCode::OP_NIL:
        return simple_instruction("OP_NIL", offset);
    case OpCode::OP_TRUE:
        return simple_instruction("OP_TRUE", offset);
    case OpCode::OP_FALSE:
        return simple_instruction("OP_FALSE", offset);
    case OpCode::OP_POP:
        return simple_instruction("OP_POP", offset);
    case OpCode::OP_SET_GLOBAL:
        return constant_instruction("OP_SET_GLOBAL", chunk, offset);
    case OpCode::OP_EQUAL:
        return simple_instruction("OP_EQUAL", offset);
    case OpCode::OP_GET_GLOBAL:
        return constant_instruction("OP_GET_GLOBAL", chunk, offset);
    case OpCode::OP_DEFINE_GLOBAL:
        return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);
    case OpCode::OP_GREATER:
        return simple_instruction("OP_GREATER", offset);
    case OpCode::OP_LESS:
        return simple_instruction("OP_LESS", offset);
    case OpCode::OP_ADD:
        return simple_instruction("OP_ADD", offset);
    case OpCode::OP_SUBTRACT:
        return simple_instruction("OP_SUBTRACT", offset);
    case OpCode::OP_MULTIPLY:
        return simple_instruction("OP_MULTIPLY", offset);
    case OpCode::OP_DIVIDE:
        return simple_instruction("OP_DIVIDE", offset);
    case OpCode::OP_NOT:
        return simple_instruction("OP_NOT", offset);
    default: {
        println("Unknown opcode {}", instruction);
        offset += 1;
        break;
    }
    }

    return offset;
}

void disassemble_chunk(const Chunk& chunk, const std::string& name) {
    println("== {} ==", name);
    usize offset = 0;
    while (offset < chunk.get_code().size()) {
        offset = disassemble_instruction(chunk, offset);
    }
}
