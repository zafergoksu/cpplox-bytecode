#include "value.h"
#include "common.h"
#include "object.h"
#include "table.h"
#include <memory>
#include <vector>

using namespace object;

namespace value {

std::string value_to_string(const Object& value) {
    return value.to_string();
}

usize ValueArray::size() const noexcept {
    return m_values.size();
}

void ValueArray::write_value(Object* value) noexcept {
    m_values.emplace_back(value);
}

const std::vector<Object*>& ValueArray::get_values() const noexcept {
    return m_values;
}

void ValueArray::clear() {
    m_values.clear();
}
} // namespace value
