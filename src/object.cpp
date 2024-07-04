#include "object.h"
#include "common.h"
#include <string>

using namespace object;

Object::Object() : m_type(ObjectType::OBJ_EMPTY) {}
Object::Object(const Object& object) : m_type(object.m_type) {}
Object::Object(ObjectType type) : m_type(type) {}

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
    return m_type == ObjectType::OBJ_NUMBER;
}

bool Object::is_string() const {
    return m_type == ObjectType::OBJ_STRING;
}

bool operator==(const Object& lhs, const Object& rhs) {
    return lhs.m_type == rhs.m_type && lhs.is_equal(rhs);
}

bool operator!=(const Object& lhs, const Object& rhs) {
    return lhs.m_type != rhs.m_type && !lhs.is_equal(rhs);
}
