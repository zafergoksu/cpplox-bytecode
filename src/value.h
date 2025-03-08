#pragma once

#include "common.h"
#include <ostream>
#include <string>
#include <vector>

namespace table {
class Table;
} // namespace table

namespace object {
class Object;
class NullObject;
class NumberObject;
class BooleanObject;
class StringObject;
} // namespace object

namespace value {

std::ostream& operator<<(std::ostream&, const object::Object& value);
std::string value_to_string(const object::Object& value);

class ValueArray {
public:
    [[nodiscard]] usize size() const noexcept;
    void write_value(object::Object* value) noexcept;
    [[nodiscard]] const std::vector<object::Object*>& get_values() const noexcept;
    void clear();

private:
    std::vector<object::Object*> m_values;
};
} // namespace value
