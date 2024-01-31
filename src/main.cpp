#include "chunk.h"
#include "common_types.h"
#include "debug.h"
#include "vm.h"

int main() {
    chunk::Chunk chunk;

    usize constant_idx = chunk.write_constant(1.2);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    chunk.write_byte(chunk::OpCode::OP_RETURN, 123);

    // disassemble_chunk(chunk, "test chunk");

    auto chunk_ptr = std::make_unique<chunk::Chunk>(chunk);
    vm::VirtualMachine vm{std::move(chunk_ptr)};
    vm.run();

    return 0;
}
