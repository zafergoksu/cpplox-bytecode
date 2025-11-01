#include "object.h"
#include "value.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

TEST(ValueArray, test_write_value) {
    value::ValueArray value_array;
    auto* number_obj_1 = new object::NumberObject{1.2};
    value_array.write_value(number_obj_1);
    auto* number_obj_2 = new object::NumberObject{2.1};
    value_array.write_value(number_obj_2);

    ASSERT_FALSE(value_array.get_values().empty());
    auto* result_1 = dynamic_cast<object::NumberObject*>(value_array.get_values()[0]);
    auto* result_2 = dynamic_cast<object::NumberObject*>(value_array.get_values()[1]);
    EXPECT_EQ(value_array.size(), 2);
    EXPECT_EQ(result_1->value, 1.2);
    EXPECT_EQ(result_2->value, 2.1);

    delete number_obj_1;
    delete number_obj_2;
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
