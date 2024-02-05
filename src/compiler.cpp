#include "compiler.h"
#include "scanner.h"

#include <string>

namespace compiler {

void compile(std::string&& source) {
    scanner::Scanner scanner{std::move(source)};
}

} // namespace compiler
