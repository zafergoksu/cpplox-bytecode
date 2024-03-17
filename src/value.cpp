#include "value.h"
#include "common.h"
#include "table.h"
#include "utility.h"
#include <ios>
#include <optional>
#include <variant>
#include <vector>

namespace value {

ObjString make_obj_string_interned(table::Table& table, std::string value) {
    u32 hash = value::ObjString::hash_string(value);

    std::optional<ObjString> interned = table.find_string(value, hash);
    if (interned) {
        return interned.value();
    }

    ObjString obj_string{std::move(value), hash};
    table.set(obj_string, nullptr);
    return obj_string;
}

ObjString make_obj_string(std::string value) {
    u32 hash = value::ObjString::hash_string(value);
    return ObjString{std::move(value), hash};
}

/*
 * For our hashing function we want three properties:
 *  - uniformity = the hashing function will spread resulting hash
 *      values out and not cluster
 *  - deterministic = the hashing function will always give out the same
 *      hash based on a key
 *  - speed = a hash function must be fast, attaining O(1) time complexity
 *      in most situations to make hash tables viable.
 * This function implements the FNV-1a hashing algorithm.
 * We xor then multiply by a prime. doing this in this order
 * will benefit avalanche characteristics, whereby a small change
 * in the key will greatly change the hash output (helping uniformity).
*/
u32 ObjString::hash_string(const std::string& key) {
    u32 hash = 2166136261u;
    for (usize i = 0; i < key.size(); i++) {
        hash ^= static_cast<u8>(key.at(i));
        hash *= 16777619;
    }
    return hash;
}

bool operator==(const ObjString& lhs, const ObjString& rhs) {
    return lhs.str == rhs.str;
}

struct ostream_value_visitor {
    ostream_value_visitor(std::ostream& os) : m_os(os) {}

    void operator()(std::nullptr_t) {
        m_os << "nil";
    }

    void operator()(bool value) {
        m_os << std::boolalpha << value;
    }

    void operator()(double value) {
        m_os << value;
    }

    void operator()(const ObjString& value) {
        m_os << value.str;
    }

    std::ostream& m_os;
};

void print_visitor::operator()(std::nullptr_t) {
    print("nil");
}

void print_visitor::operator()(bool value) {
    print("{}", value);
}

void print_visitor::operator()(double value) {
    print("{:g}", value);
}

void print_visitor::operator()(const ObjString& value) {
    print("{}", value.str);
}

std::string value_to_string_visitor::operator()(std::nullptr_t) {
    return "nil";
}

std::string value_to_string_visitor::operator()(bool value) {
    if (value) {
        return "true";
    }
    return "false";
}

std::string value_to_string_visitor::operator()(double value) {
    return std::to_string(value);
}

std::string value_to_string_visitor::operator()(const ObjString& value) {
    return value.str;
}

std::ostream& operator<<(std::ostream& os, const Value& value) {
    std::visit(ostream_value_visitor{os}, value);
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

void ValueArray::clear() {
    m_values.clear();
}
} // namespace value
