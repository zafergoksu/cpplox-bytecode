#include "heap.h"
#include "common.h"
#include "object.h"

using ds::Heap;
using object::StringObject;

StringObject* Heap::make_obj_string(const std::string& value) noexcept {
    const u32 hash = StringObject::hash_string(value);

    if (StringObject* interned = m_strings.find_string(value, hash); interned != nullptr) {
        return interned;
    }

    auto string_object = make_object<StringObject>(value);
    m_strings.set(string_object, nullptr);
    return string_object;
}