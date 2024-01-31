#include "chunk.h"
#include "common.h"
#include "vm.h"

int main() {
    chunk::Chunk chunk;

    usize constant_idx = chunk.write_constant(1.2);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    constant_idx = chunk.write_constant(3.4);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    chunk.write_byte(chunk::OpCode::OP_ADD, 123);

    constant_idx = chunk.write_constant(5.6);
    chunk.write_byte(chunk::OpCode::OP_CONSTANT, 123);
    chunk.write_byte(constant_idx, 123);

    chunk.write_byte(chunk::OpCode::OP_DIVIDE, 123);
    chunk.write_byte(chunk::OpCode::OP_NEGATE, 123);
    chunk.write_byte(chunk::OpCode::OP_RETURN, 123);

    auto chunk_ptr = std::make_unique<chunk::Chunk>(chunk);
    vm::VirtualMachine vm{std::move(chunk_ptr)};
    vm.run();

    // write any unit tests
    // write e2e test with files

    return 0;
}
