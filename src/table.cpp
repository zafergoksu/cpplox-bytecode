#include "table.h"
#include "common.h"
#include "value.h"
#include <cstddef>
#include <optional>
#include <string>
#include <variant>

using namespace value;

namespace table {
Table::Table() : m_entries{k_initial_capacity} {}

bool Table::set(ObjString& key, Value value) {
    Entry* entry = find_entry(key);
    bool is_new_key = entry->key == std::nullopt;
    entry->key = key;
    entry->value = value;
    return is_new_key;
}

bool Table::get(ObjString& key, Value& value) {
    if (m_entries.size() == 0) {
        return false;
    }

    Entry* entry = find_entry(key);
    if (!entry->key) {
        return false;
    }

    value = entry->value;
    return true;
}

bool Table::del(ObjString& key) {
    if (m_entries.size() == 0) {
        return false;
    }

    Entry* entry = find_entry(key);
    if (!entry->key) {
        return false;
    }

    // Place a tombstone. A tombstone is a sentinel value to allow continue
    // looking down the chain in linear probing. We can delete a value in the middle
    // of the chain and would orphan the succeeding colliding values, therefore,
    // a tombstone allows us to continue linear probing until we find the latest collided value

    entry->key = std::nullopt;
    entry->value = true;
    return true;
}

void Table::add_all(Table& to) {
    for (u32 i = 0; i < m_entries.capacity(); i++) {
        Entry& entry = m_entries[i];
        if (entry.key) {
            to.set(entry.key.value(), entry.value);
        }
    }
}

Entry* Table::find_entry(ObjString& key) {
    auto capacity = m_entries.capacity();
    u32 index = key.hash % capacity;
    Entry* tombstone = nullptr;
    while (true) {
        Entry* entry = &m_entries[index];
        if (!entry->key) {
            if (std::holds_alternative<std::nullptr_t>(entry->value)) {
                // This entry is truely empty
                // return a tombstone slot if we encountered one earlier
                return tombstone != nullptr ? tombstone : entry;
            } else {
                // We found a tombstone, continue looking
                if (!tombstone) {
                    tombstone = entry;
                }
            }
        } else if (entry->key == key) {
            // We found our key
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

std::optional<ObjString> Table::find_string(const std::string& value, u32 hash) {
    if (m_entries.size() == 0) {
        return std::nullopt;
    }

    u32 index = hash % m_entries.capacity();

    while (true) {
        Entry& entry = m_entries[index];
        if (!entry.key) {
            // Stop if we find an empty non-tombstone entry.
            if (std::holds_alternative<std::nullptr_t>(entry.value)) {
                return std::nullopt;
            }
        } else if (entry.key->str.length() && entry.key->hash == hash && entry.key->str == value) {
            return entry.key;
        }

        index = (index + 1) % m_entries.capacity();
    }
}
} // namespace table
