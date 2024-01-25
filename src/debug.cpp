#include "debug.h"
#include "chunk.h"
#include "common_types.h"
#include "utility.h"
#include "value.h"
#include <string>

namespace {
usize simple_instruction(const std::string& name, usize offset) {
    println("{}", "OP_RETURN");
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

void disassemble_chunk(const chunk::Chunk& chunk, const std::string& name) {
    println("== {} ==", name);
    for (usize offset = 0; offset < chunk.get_code().size();) {
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
        case chunk::OpCode::OP_CONSTANT:
            offset = ::constant_instruction("OP_CONSTANT", chunk, offset);
            break;
        default: {
            println("Unknown opcode {}", instruction);
            offset += 1;
            break;
        }
        }
    }
}