#include "table.h"
#include "common.h"
#include "value.h"
#include <cstddef>
#include <string>
#include <variant>

using namespace value;

namespace table {
Table::Table() : m_count{0}, m_capacity{k_initial_capacity}, m_entries{std::make_unique<Entry[]>(k_initial_capacity)} {}

bool Table::set(ObjString* key, Value value) {
    if (m_count + 1 > static_cast<u32>(static_cast<float>(m_capacity) * k_max_load)) {
        adjust_capacity(m_capacity * 2);
    }

    Entry* entry = find_entry(m_entries, m_capacity, key);
    bool is_new_key = entry->key == nullptr;

    // we are not counting tombstones if we get a free entry to use that is a
    // tombstone
    if (is_new_key && std::holds_alternative<std::nullptr_t>(entry->value)) {
        m_count++;
    }

    entry->key = key;
    entry->value = value;
    return is_new_key;
}

bool Table::get(ObjString* key, Value& value) {
    if (m_count == 0) {
        return false;
    }

    Entry* entry = find_entry(m_entries, m_capacity, key);
    if (entry->key == nullptr) {
        return false;
    }

    value = entry->value;
    return true;
}

bool Table::del(ObjString* key) {
    if (m_count == 0) {
        return false;
    }

    Entry* entry = find_entry(m_entries, m_capacity, key);
    if (entry->key == nullptr) {
        return false;
    }

    // Place a tombstone. A tombstone is a sentinel value to allow continue
    // looking down the chain in linear probing. We can delete a value in the middle
    // of the chain and would orphan the succeeding colliding values, therefore,
    // a tombstone allows us to continue linear probing until we find the latest collided value

    entry->key = nullptr;
    entry->value = true;
    return true;
}

void Table::add_all(Table& to) {
    for (u32 i = 0; i < m_capacity; i++) {
        Entry* entry = &m_entries[i];
        if (entry->key != nullptr) {
            to.set(entry->key, entry->value);
        }
    }
}

Entry* Table::find_entry(const std::unique_ptr<Entry[]>& entries, u32 capacity, ObjString* key) {
    u32 index = key->hash % capacity;
    Entry* tombstone = nullptr;
    while (true) {
        Entry* entry = &entries[index];
        if (entry->key == nullptr) {
            if (std::holds_alternative<std::nullptr_t>(entry->value)) {
                // This entry is truely empty
                // return a tombstone slot if we encountered one earlier
                return tombstone != nullptr ? tombstone : entry;
            } else {
                // We found a tombstone, continue looking
                if (tombstone == nullptr) {
                    tombstone = entry;
                }
            }
        } else if (entry->key == key) {
            // We found our key
            return entry;
        }
    }

    index = (index + 1) % capacity;
}

ObjString* Table::find_string(const std::string& value, u32 hash) {
    if (m_count == 0) {
        return nullptr;
    }

    u32 index = hash % m_capacity;

    while (true) {
        Entry* entry = &m_entries[index];
        if (entry->key == nullptr) {
            // Stop if we find an empty non-tombstone entry.
            if (std::holds_alternative<std::nullptr_t>(entry->value)) {
                return nullptr;
            }
        } else if (entry->key->str.length() && entry->key->hash == hash && entry->key->str == value) {
            return entry->key;
        }

        index = (index + 1) % m_capacity;
    }
}

void Table::adjust_capacity(u32 new_capacity) {
    auto entries = std::make_unique<Entry[]>(new_capacity);

    // by rebuilding the table when adjusting the capacity,
    // we reset the count to disard tombstones
    m_count = 0;
    for (u32 i = 0; i < m_capacity; i++) {
        Entry* entry = &m_entries[i];
        if (entry->key == nullptr) {
            continue;
        }

        Entry* dest = find_entry(entries, new_capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        m_count++;
    }

    m_capacity = new_capacity;
    m_entries = std::move(entries);
}

} // namespace table
