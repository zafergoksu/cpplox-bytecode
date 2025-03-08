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

    object::StringObject* make_obj_string(std::string value) noexcept;

private:
    ds::List<object::Object> m_objects;
    table::Table m_strings;
};
} // namespace ds