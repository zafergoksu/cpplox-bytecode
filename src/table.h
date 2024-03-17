#pragma once

#include "common.h"
#include "value.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace table {

struct Entry {
    std::optional<value::ObjString> key;
    value::Value value;
};

class Table {
public:
    Table();

    bool set(value::ObjString& key, value::Value value);
    bool get(value::ObjString& key, value::Value& value);
    bool del(value::ObjString& key);
    std::optional<value::ObjString> find_string(const std::string& value, u32 hash);
    void add_all(Table& to);
    Entry* find_entry(value::ObjString& key);

private:
    static constexpr u32 k_initial_capacity = 8;
    static constexpr float k_max_load = 0.75f;

    void adjust_capacity(u32 capacity);

    // std::unique_ptr<Entry[]> m_entries;
    std::vector<Entry> m_entries;
};
} // namespace table
