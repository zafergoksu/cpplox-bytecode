#pragma once

#include "list.h"
#include "object.h"
#include "table.h"
#include <utility>

namespace ds {
class Heap {
public:
    template<typename T, typename... Args>
    T* make_object(Args&&... args) noexcept {
        const auto obj = new T{std::forward<Args>(args)...};
        m_objects.insert(obj);
        return obj;
    }

    object::StringObject* make_obj_string(const std::string& value) noexcept;
    object::NullObject* nil();
    object::BooleanObject* boolean(bool value);

private:
    ds::List<object::Object> m_objects;
    table::Table m_strings;
    object::NullObject* m_nil = nullptr;
    object::BooleanObject* m_true = nullptr;
    object::BooleanObject* m_false = nullptr;
};
} // namespace ds
