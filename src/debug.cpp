#include "debug.h"
#include "chunk.h"
#include "common.h"
#include "utility.h"
#include "value.h"
#include <string>

namespace {
usize simple_instruction(const std::string& name, usize offset) {
    println("{}", name);
    return offset + 1;
}

usize constant_instruction(const std::string& name, const chunk::Chunk& chunk, usize offset) {
    u8 constant = chunk.get_code().at(offset + 1);
    print("{:16s} {:4d} '", name, constant);
    auto value = chunk.get_constants().get_values().at(constant);
    println("{:g}'", value);
    return offset + 2;
}
} // namespace

usize disassemble_instruction(const chunk::Chunk& chunk, usize offset) {
    print("{:04d} ", offset);
    if (offset > 0 && chunk.get_lines().at(offset) == chunk.get_lines().at(offset - 1)) {
        print("\t| ");
    } else {
        print("{:4d} ", chunk.get_lines().at(offset));
    }

    u8 instruction = chunk.get_code().at(offset);
    switch (instruction) {
    case chunk::OpCode::OP_RETURN:
        offset = ::simple_instruction("OP_RETURN", offset);
        break;
    case chunk::OpCode::OP_NEGATE:
        return simple_instruction("OP_NEGATE", offset);
    case chunk::OpCode::OP_CONSTANT:
        offset = ::constant_instruction("OP_CONSTANT", chunk, offset);
        break;
    case chunk::OpCode::OP_ADD:
        return simple_instruction("OP_ADD", offset);
    case chunk::OpCode::OP_SUBTRACT:
        return simple_instruction("OP_SUBTRACT", offset);
    case chunk::OpCode::OP_MULTIPLY:
        return simple_instruction("OP_MULTIPLY", offset);
    case chunk::OpCode::OP_DIVIDE:
        return simple_instruction("OP_DIVIDE", offset);
    default: {
        println("Unknown opcode {}", instruction);
        offset += 1;
        break;
    }
    }

    return offset;
}

void disassemble_chunk(const chunk::Chunk& chunk, const std::string& name) {
    println("== {} ==", name);
    usize offset = 0;
    while (offset < chunk.get_code().size()) {
        offset = disassemble_instruction(chunk, offset);
    }
}
