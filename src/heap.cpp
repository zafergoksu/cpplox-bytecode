#include "heap.h"
#include "common.h"
#include "object.h"

using ds::Heap;
using object::StringObject;

StringObject* Heap::make_obj_string(const std::string& value) {
    const u32 hash = StringObject::hash_string(value);

    if (StringObject* interned = m_strings.find_string(value, hash); interned != nullptr) {
        return interned;
    }

    auto string_object = make_object<StringObject>(value);
    m_strings.set(string_object, nullptr);
    return string_object;
}

object::NullObject* Heap::nil() {
    if (m_nil == nullptr) {
        m_nil = make_object<object::NullObject>();
    }

    return m_nil;
}

object::BooleanObject* Heap::boolean(bool value) {
    object::BooleanObject*& singleton = value ? m_true : m_false;
    if (singleton == nullptr) {
        singleton = make_object<object::BooleanObject>(value);
    }

    return singleton;
}
