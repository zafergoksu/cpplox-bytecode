#include "object.h"
#include "chunk.h"
#include "common.h"
#include <string>

using namespace object;
using namespace chunk;

Object::Object() : type(ObjectType::OBJ_EMPTY) {}
Object::Object(const Object& object) : type(object.type) {}
Object::Object(ObjectType type) : type(type) {}

std::string Object::to_string() const {
    return std::string{};
}

bool Object::is_falsey() const {
    return true;
}

bool Object::is_truthy() const {
    return false;
}

bool Object::is_equal(const Object& other) const {
    return true;
}

bool Object::is_numeric() const {
    return type == ObjectType::OBJ_NUMBER;
}

bool Object::is_string() const {
    return type == ObjectType::OBJ_STRING;
}

bool operator==(const Object& lhs, const Object& rhs) {
    return lhs.type == rhs.type && lhs.is_equal(rhs);
}

bool operator!=(const Object& lhs, const Object& rhs) {
    return lhs.type != rhs.type && !lhs.is_equal(rhs);
}

NullObject::NullObject()
    : Object{ObjectType::OBJ_NULL} {}

std::string NullObject::to_string() const {
    return "nil";
}

bool NullObject::is_falsey() const {
    return true;
}

bool NullObject::is_truthy() const {
    return false;
}

bool NullObject::is_equal(const Object& other) const {
    return other.type == ObjectType::OBJ_NULL;
}

NumberObject::NumberObject(double v) : Object{ObjectType::OBJ_NUMBER}, value{v} {}

std::string NumberObject::to_string() const {
    return std::to_string(value);
}

bool NumberObject::is_falsey() const {
    return value == 0;
}

bool NumberObject::is_truthy() const {
    return value != 0;
}

bool NumberObject::is_equal(const Object& other) const {
    if (other.type == type) {
        const auto& obj = static_cast<const NumberObject&>(other);
        return obj.value == value;
    }
    return false;
}

BooleanObject::BooleanObject(bool v) : Object{ObjectType::OBJ_BOOLEAN}, value(v) {}

std::string BooleanObject::to_string() const {
    return value ? "true" : "false";
}

bool BooleanObject::is_falsey() const {
    return value == false;
}

bool BooleanObject::is_truthy() const {
    return value == true;
}

bool BooleanObject::is_equal(const Object& other) const {
    if (other.type == type) {
        const auto& obj = static_cast<const BooleanObject&>(other);
        return obj.value == value;
    }
    return false;
}

StringObject::StringObject(const std::string& v) : Object{ObjectType::OBJ_STRING}, value(v), hash(hash_string(v)) {}

std::string StringObject::to_string() const {
    return value;
}

bool StringObject::is_falsey() const {
    return value.empty();
}

bool StringObject::is_truthy() const {
    return !value.empty();
}

bool StringObject::is_equal(const Object& other) const {
    if (other.type == type) {
        const auto& obj = static_cast<const StringObject&>(other);
        return obj.value == value;
    }
    return false;
}

FunctionObject::FunctionObject(int arity, FunctionType func_type, std::shared_ptr<Chunk> chunk, std::shared_ptr<StringObject> name)
    : arity{arity},
      func_type{func_type},
      chunk{std::move(chunk)},
      name{std::move(name)} {}

std::string FunctionObject::to_string() const {
    if (name == nullptr) {
        return "<script>";
    }
    return "<function '" + name->to_string() + "'>";
}

bool FunctionObject::is_falsey() const {
    return false;
}

bool FunctionObject::is_truthy() const {
    return true;
}

bool FunctionObject::is_equal(const Object& other) const {
    if (other.type == type) {
        const auto& obj = static_cast<const FunctionObject&>(other);
        return obj.arity == arity && obj.chunk == chunk && obj.name == name && obj.name->to_string() == name->to_string();
    }

    return false;
}

/*
 * For our hashing function we want three properties:
 *  - uniformity = the hashing function will spread resulting hash
 *      values out and not cluster
 *  - deterministic = the hashing function will always give out the same
 *      hash based on a key
 *  - speed = a hash function must be fast, attaining O(1) time complexity
 *      in most situations to make hash tables viable.
 * This function implements the FNV-1a hashing algorithm.
 * We xor then multiply by a prime. doing this in this order
 * will benefit avalanche characteristics, whereby a small change
 * in the key will greatly change the hash output (helping uniformity).
*/
u32 StringObject::hash_string(const std::string& key) {
    u32 hash = 2166136261u;
    for (usize i = 0; i < key.size(); i++) {
        hash ^= static_cast<u8>(key.at(i));
        hash *= 16777619;
    }
    return hash;
}
