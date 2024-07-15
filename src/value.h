#pragma once

#include "common.h"
#include <memory>
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

std::shared_ptr<object::StringObject> make_obj_string_interned(table::Table& table, std::string value);

std::ostream& operator<<(std::ostream&, const object::Object& value);
std::string value_to_string(const object::Object& value);

class ValueArray {
public:
    [[nodiscard]] usize size() const;
    void write_value(std::shared_ptr<object::Object> value);
    [[nodiscard]] const std::vector<std::shared_ptr<object::Object>>& get_values() const;
    void clear();

private:
    std::vector<std::shared_ptr<object::Object>> m_values;
};
} // namespace value
