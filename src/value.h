#pragma once

#include "common.h"
#include <cstddef>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace table {
class Table;
}

namespace value {

struct ObjString {
    std::string str;
    u32 hash;

    static u32 hash_string(const std::string& value);
};

ObjString make_obj_string_interned(table::Table& table, std::string value);
ObjString make_obj_string(std::string value);

bool operator==(const ObjString& lhs, const ObjString& rhs);

using Value = std::variant<
    // nil type
    std::nullptr_t,

    // Primitive types
    bool,
    double,

    ObjString>;

std::ostream& operator<<(std::ostream&, const Value& value);
std::string value_to_string(const Value& value);

struct value_to_string_visitor {
    std::string operator()(std::nullptr_t);
    std::string operator()(bool value);
    std::string operator()(double value);
    std::string operator()(const ObjString& value);
};

struct print_visitor {
    void operator()(std::nullptr_t);
    void operator()(bool value);
    void operator()(double value);
    void operator()(const ObjString& value);
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
