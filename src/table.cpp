#include "table.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include <memory>
#include <string>

using namespace value;
using namespace object;

namespace table {
Table::Table() : m_entries{k_initial_capacity} {}

bool Table::set(std::shared_ptr<StringObject> key, std::shared_ptr<Object> value) {
    Entry* entry = find_entry(key);
    bool is_new_key = entry->key == nullptr;
    entry->key = key;
    entry->value = value;
    return is_new_key;
}

bool Table::get(std::shared_ptr<StringObject> key, std::shared_ptr<Object>& value) {
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

bool Table::del(std::shared_ptr<StringObject> key) {
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

    entry->key = nullptr;
    entry->value = std::make_shared<BooleanObject>(true);
    return true;
}

void Table::add_all(Table& to) {
    for (u32 i = 0; i < m_entries.capacity(); i++) {
        Entry& entry = m_entries[i];
        if (entry.key != nullptr) {
            to.set(entry.key, entry.value);
        }
    }
}

Entry* Table::find_entry(std::shared_ptr<StringObject> key) {
    auto capacity = m_entries.capacity();
    u32 index = key->hash % capacity;
    Entry* tombstone = nullptr;
    while (true) {
        Entry* entry = &m_entries[index];
        if (!entry->key) {
            if (entry->value == nullptr || entry->value->type == ObjectType::OBJ_NULL) {
                // This entry is truely empty
                // return a tombstone slot if we encountered one earlier
                return tombstone != nullptr ? tombstone : entry;
            } else {
                // We found a tombstone, continue looking
                if (!tombstone) {
                    tombstone = entry;
                }
            }
        } else if (entry->key->is_equal(*key)) {
            // We found our key
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

std::shared_ptr<StringObject> Table::find_string(const std::string& value, u32 hash) {
    if (m_entries.size() == 0) {
        return nullptr;
    }

    u32 index = hash % m_entries.capacity();

    while (true) {
        Entry& entry = m_entries[index];
        if (!entry.key) {
            // Stop if we find an empty non-tombstone entry.
            if (entry.value == nullptr || entry.value->type == ObjectType::OBJ_NULL) {
                return nullptr;
            }
        } else if (entry.key->value.length() && entry.key->hash == hash && entry.key->value == value) {
            return entry.key;
        }

        index = (index + 1) % m_entries.capacity();
    }
}
} // namespace table
