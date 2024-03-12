#pragma once

#include "common.h"
#include <cstddef>
#include <ostream>
#include <variant>
#include <vector>

namespace value {

using Value = std::variant<
    // nil type
    std::nullptr_t,

    // Primitive types
    bool,
    double>;

std::ostream& operator<<(std::ostream&, const Value& value);
std::string value_to_string(const Value& value);

class ValueArray {
public:
    [[nodiscard]] usize size() const;
    void write_value(Value value);
    [[nodiscard]] const std::vector<Value>& get_values() const;

private:
    std::vector<Value> m_values;
};
} // namespace value
