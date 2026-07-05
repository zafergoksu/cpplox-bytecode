#pragma once

#include "common.h"
#include "object.h"
#include "value.h"
#include <memory>
#include <string>
#include <vector>

namespace table {

struct Entry {
    Entry();

    object::StringObject* key;
    object::Object* value;
};

class Table {
public:
    Table();

    bool set(object::StringObject* key, object::Object* value);
    object::Object* get(object::StringObject* key);
    bool del(object::StringObject* key);
    object::StringObject* find_string(const std::string& value, u32 hash);
    void add_all(Table& to);
    Entry* find_entry(object::StringObject* key);

private:
    static constexpr u32 k_initial_capacity = 8;
    static constexpr float k_max_load = 0.75f;

    void adjust_capacity(u32 capacity);

    u32 m_count;
    std::vector<Entry> m_entries;
};
} // namespace table
