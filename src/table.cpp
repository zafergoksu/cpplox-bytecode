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
    if (m_entries.size() + 1 > static_cast<u32>(static_cast<float>(m_entries.capacity()) * k_max_load)) {
        // adjust_capacity(m_capacity * 2);
        m_entries.resize(m_entries.capacity() * 2);
    }

    Entry* entry = find_entry(key);
    bool is_new_key = entry->key == std::nullopt;

    // we are not counting tombstones if we get a free entry to use that is a
    // tombstone
    // if (is_new_key && std::holds_alternative<std::nullptr_t>(entry.value)) {
    //     m_count++;
    // }

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

// void Table::adjust_capacity(u32 new_capacity) {
//     auto entries = std::make_unique<Entry[]>(new_capacity);
//
//     // by rebuilding the table when adjusting the capacity,
//     // we reset the count to disard tombstones
//     m_count = 0;
//     for (u32 i = 0; i < m_capacity; i++) {
//         Entry* entry = &m_entries[i];
//         if (entry->key == nullptr) {
//             continue;
//         }
//
//         Entry* dest = find_entry(entries, new_capacity, entry->key);
//         dest->key = entry->key;
//         dest->value = entry->value;
//         m_count++;
//     }
//
//     m_capacity = new_capacity;
//     m_entries = std::move(entries);
// }

} // namespace table
