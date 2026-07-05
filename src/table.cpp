#include "table.h"
#include "common.h"
#include "object.h"
#include "value.h"
#include <string>

using namespace value;
using namespace object;

namespace {
static BooleanObject s_tombstone{true};
} // namespace

namespace table {

Entry::Entry()
    : key{nullptr}, value{nullptr} {}

Table::Table() : m_count{0}, m_entries{k_initial_capacity} {}

bool Table::set(StringObject* key, Object* value) {
    if (m_count + 1 > m_entries.size() * k_max_load) {
        u32 new_capacity = m_entries.size() * 2;
        adjust_capacity(new_capacity);
    }

    Entry* entry = find_entry(key);
    bool is_new_key = entry->key == nullptr;

    // only increment size not including tombstone
    if (is_new_key && (entry->value == nullptr || entry->value->type == ObjectType::OBJ_NULL)) {
        m_count++;
    }

    entry->key = key;
    entry->value = value;
    return is_new_key;
}

Object* Table::get(StringObject* key) {
    if (m_entries.size() == 0) {
        return nullptr;
    }

    Entry* entry = find_entry(key);
    if (!entry->key) {
        return nullptr;
    }

    return entry->value;
}

bool Table::del(StringObject* key) {
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
    entry->value = &s_tombstone;
    return true;
}

void Table::add_all(Table& to) {
    for (const Entry& entry : m_entries) {
        if (entry.key != nullptr) {
            to.set(entry.key, entry.value);
        }
    }
}

void Table::adjust_capacity(u32 new_capacity) {
    std::vector<Entry> new_entries(new_capacity);

    for (auto& entry : m_entries) {
        // ignore tombstones
        if (entry.key == nullptr) {
            continue;
        }

        u32 index = entry.key->hash % new_capacity;
        while (true) {
            Entry* dest = &new_entries[index];
            dest->key = entry.key;
            dest->value = entry.value;
            m_count++;
            break;
        }

        index = (index + 1) % new_capacity;
    }

    m_entries = std::move(new_entries);
}

Entry* Table::find_entry(StringObject* key) {
    auto capacity = m_entries.size();
    u32 index = key->hash % capacity;
    Entry* tombstone = nullptr;
    while (true) {
        Entry* entry = &m_entries[index];
        if (entry->key == nullptr) {
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

StringObject* Table::find_string(const std::string& value, u32 hash) {
    if (m_entries.size() == 0) {
        return nullptr;
    }

    u32 index = hash % m_entries.size();

    while (true) {
        Entry& entry = m_entries[index];
        if (entry.key == nullptr) {
            // Stop if we find an empty non-tombstone entry.
            if (entry.value == nullptr || entry.value->type == ObjectType::OBJ_NULL) {
                return nullptr;
            }
        } else if (entry.key->value.length() && entry.key->hash == hash && entry.key->value == value) {
            return entry.key;
        }

        index = (index + 1) % m_entries.size();
    }
}
} // namespace table
