#include "heap.h"
#include "common.h"
#include "object.h"

using ds::Heap;
using object::StringObject;

StringObject* Heap::make_obj_string(std::string value) noexcept {
    u32 hash = StringObject::hash_string(value);
    StringObject* interned = m_strings.find_string(value, hash);

    if (interned != nullptr) {
        return interned;
    }

    auto string_object = make_object<StringObject>(std::move(value));
    m_strings.set(string_object, nullptr);
    return string_object;
}