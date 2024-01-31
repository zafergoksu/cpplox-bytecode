#pragma once

#include "chunk.h"
#include "common.h"
#include <string>

usize disassemble_instruction(const chunk::Chunk&, usize offset);
void disassemble_chunk(const chunk::Chunk& chunk, const std::string& name);
