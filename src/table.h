#pragma once

#include "common.h"
#include "value.h"
#include <memory>
#include <string>
#include <vector>

namespace table {

struct Entry {
    value::ObjString* key;
    value::Value value;
};

class Table {
public:
    Table();

    bool set(value::ObjString* key, value::Value value);
    bool get(value::ObjString* key, value::Value& value);
    bool del(value::ObjString* key);
    value::ObjString* find_string(const std::string& value, u32 hash);
    void add_all(Table& to);
    Entry* find_entry(const std::unique_ptr<Entry[]>& entries, u32 capacity, value::ObjString* key);

private:
    static constexpr u32 k_initial_capacity = 8;
    static constexpr float k_max_load = 0.75f;

    void adjust_capacity(u32 capacity);

    u32 m_count;
    u32 m_capacity;
    std::unique_ptr<Entry[]> m_entries;
};
} // namespace table
