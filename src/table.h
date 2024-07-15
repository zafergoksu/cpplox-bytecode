#pragma once

#include "common.h"
#include "object.h"
#include "value.h"
#include <memory>
#include <string>
#include <vector>

namespace table {

struct Entry {
    std::shared_ptr<object::StringObject> key;
    std::shared_ptr<object::Object> value;
};

class Table {
public:
    Table();

    bool set(std::shared_ptr<object::StringObject> key, std::shared_ptr<object::Object> value);
    bool get(std::shared_ptr<object::StringObject> key, std::shared_ptr<object::Object> value);
    bool del(std::shared_ptr<object::StringObject> key);
    std::shared_ptr<object::StringObject> find_string(const std::string& value, u32 hash);
    void add_all(Table& to);
    Entry* find_entry(std::shared_ptr<object::StringObject> key);

private:
    static constexpr u32 k_initial_capacity = 8;
    static constexpr float k_max_load = 0.75f;

    void adjust_capacity(u32 capacity);

    // std::unique_ptr<Entry[]> m_entries;
    std::vector<Entry> m_entries;
};
} // namespace table
