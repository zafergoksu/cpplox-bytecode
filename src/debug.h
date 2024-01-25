#ifndef CPPLOX_BYTECODE_SRC_DEBUG_H
#define CPPLOX_BYTECODE_SRC_DEBUG_H

#include "chunk.h"
#include "common_types.h"
#include <string>

void disassemble_chunk(const chunk::Chunk& chunk, const std::string& name);

#endif //CPPLOX_BYTECODE_SRC_DEBUG_H
