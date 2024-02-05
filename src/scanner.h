#pragma once

#include "common.h"

#include <string>

namespace scanner {

class Scanner {
public:
    Scanner(std::string source);

private:
    usize m_start_idx;
    usize m_current_idx;
    usize m_line;
    std::string m_source;
};

} // namespace scanner
