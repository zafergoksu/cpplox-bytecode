#pragma once

#include "common.h"
#include <string>

namespace object {

enum class ObjectType {
    OBJ_EMPTY = 0,
    OBJ_NULL,
    OBJ_NUMBER,
    OBJ_BOOLEAN,
    OBJ_STRING
};

struct Object {
    Object();
    Object(const Object& obj);
    Object(const ObjectType type);

    virtual std::string to_string() const;
    virtual bool is_falsey() const;
    virtual bool is_truthy() const;
    virtual bool is_equal(const Object& other) const;
    inline bool is_numeric() const;
    inline bool is_string() const;

protected:
    friend bool operator==(const Object& lhs, const Object& rhs);
    friend bool operator!=(const Object& lhs, const Object& rhs);

public:
    const ObjectType m_type;
};

bool operator==(const Object& lhs, const Object& rhs);
bool operator!=(const Object& lhs, const Object& rhs);

struct NullObject : public Object {
    NullObject();

    std::string to_string() const override;
    bool is_falsey() const override;
    bool is_truthy() const override;
    bool is_equal(const Object& other) const override;
};

struct NumberObject : public Object {
    NumberObject(double value);

    std::string to_string() const override;
    bool is_falsey() const override;
    bool is_truthy() const override;
    bool is_equal(const Object& other) const override;

    double value;
};

struct BooleanObject : public Object {
    BooleanObject(bool value);

    std::string to_string() const override;
    bool is_falsey() const override;
    bool is_truthy() const override;
    bool is_equal(const Object& other) const override;

    bool value;
};

struct StringObject : public Object {
    StringObject(const std::string& value);

    std::string to_string() const override;
    bool is_falsey() const override;
    bool is_truthy() const override;
    bool is_equal(const Object& other) const override;

    static u32 hash_string(const std::string& value);

    std::string value;
    u32 hash;
};

/*
 * TODO(zafergoksu):
 *  - make sure to implement these functions
 *  - move print visitor and others from value.h to here
 *  - refactor value array
 *  - refactor vm to not use `std::holds_alternative`
 *  - refactor table.h a bit with new string object
 *  - fix tests
*/

} // namespace object
