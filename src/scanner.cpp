#include "scanner.h"

namespace scanner {

Scanner::Scanner(std::string source)
    : m_start_idx{0},
      m_current_idx{0},
      m_line{1},
      m_source{std::move(source)} {}

} // namespace scanner
