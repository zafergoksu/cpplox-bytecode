#include "chunk.h"
#include "common_types.h"
#include "debug.h"
#include <iostream>

int main() {
    chunk::Chunk chunk;

    usize constant_idx = chunk.write_constant(1.2);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    chunk.write_byte(chunk::OpCode::OP_RETURN, 123);

    disassemble_chunk(chunk, "test chunk");

    return 0;
}