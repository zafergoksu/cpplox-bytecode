#include "value.h"
#include "common_types.h"
#include <vector>

namespace value {
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
