#pragma once

#include "common.h"
#include <cstddef>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace value {

using Value = std::variant<
    // nil type
    std::nullptr_t,

    // Primitive types
    bool,
    double,

    std::string>;

std::ostream& operator<<(std::ostream&, const Value& value);
std::string value_to_string(const Value& value);

struct value_to_string_visitor {
    std::string operator()(std::nullptr_t);
    std::string operator()(bool value);
    std::string operator()(double value);
    std::string operator()(const std::string& value);
};

struct print_visitor {
    void operator()(std::nullptr_t);
    void operator()(bool value);
    void operator()(double value);
    void operator()(const std::string& value);
};

class ValueArray {
public:
    [[nodiscard]] usize size() const;
    void write_value(Value value);
    [[nodiscard]] const std::vector<Value>& get_values() const;
    void clear();

private:
    std::vector<Value> m_values;
};
} // namespace value
