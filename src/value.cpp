#include "value.h"
#include "common.h"
#include "object.h"
#include "table.h"
#include <memory>
#include <vector>

using namespace object;

namespace value {

std::shared_ptr<StringObject> make_obj_string_interned(table::Table& table, std::string value) {
    /* u32 hash = value::ObjString::hash_string(value); */
    /**/
    /* std::optional<ObjString> interned = table.find_string(value, hash); */
    /* if (interned) { */
    /*     return interned.value(); */
    /* } */
    /**/
    /* ObjString obj_string{std::move(value), hash}; */
    /* table.set(obj_string, nullptr); */
    /* return obj_string; */

    u32 hash = StringObject::hash_string(value);
    std::shared_ptr<StringObject> interned = table.find_string(value, hash);

    if (interned != nullptr) {
        return interned;
    }

    auto string_object = std::make_shared<StringObject>(value);
    table.set(string_object, nullptr);
    return string_object;
}

std::string value_to_string(const Object& value) {
    return value.to_string();
}

usize ValueArray::size() const {
    return m_values.size();
}

void ValueArray::write_value(std::shared_ptr<Object> value) {
    m_values.emplace_back(std::move(value));
}

const std::vector<std::shared_ptr<Object>>& ValueArray::get_values() const {
    return m_values;
}

void ValueArray::clear() {
    m_values.clear();
}
} // namespace value
