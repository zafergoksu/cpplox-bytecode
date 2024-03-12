#include "value.h"
#include "common.h"
#include <ios>
#include <variant>
#include <vector>

namespace value {

struct print_visitor {
    print_visitor(std::ostream& os) : m_os(os) {}

    void operator()(std::nullptr_t) {
        m_os << "nil";
    }

    void operator()(bool value) {
        m_os << std::boolalpha << value;
    }

    void operator()(double value) {
        m_os << value;
    }

    std::ostream& m_os;
};

struct value_to_string_visitor {
    std::string operator()(std::nullptr_t) {
        return "nil";
    }

    std::string operator()(bool value) {
        if (value) {
            return "true";
        }
        return "false";
    }

    std::string operator()(double value) {
        return std::to_string(value);
    }
};

std::ostream& operator<<(std::ostream& os, const Value& value) {
    std::visit(print_visitor{os}, value);
    return os;
}

std::string value_to_string(const Value& value) {
    return std::visit(value_to_string_visitor{}, value);
}

usize ValueArray::size() const {
    return m_values.size();
}

void ValueArray::write_value(Value value) {
    m_values.emplace_back(value);
}

const std::vector<Value>& ValueArray::get_values() const {
    return m_values;
}
} // namespace value
