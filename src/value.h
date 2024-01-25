#ifndef CPPLOX_BYTECODE_SRC_VALUE_H
#define CPPLOX_BYTECODE_SRC_VALUE_H

#include "common_types.h"
#include <vector>

namespace value {
typedef double Value;

class ValueArray {
public:
    [[nodiscard]] usize size() const;
    void write_value(Value value);
    [[nodiscard]] const std::vector<Value>& get_values() const;

private:
    std::vector<Value> m_values;
};
} // namespace value

#endif //CPPLOX_BYTECODE_SRC_VALUE_H
