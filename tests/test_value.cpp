#include "object.h"
#include "value.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

TEST(ValueArray, test_write_value) {
    value::ValueArray value_array;
    value_array.write_value(std::make_shared<object::NumberObject>(1.2));
    value_array.write_value(std::make_shared<object::NumberObject>(2.1));

    ASSERT_FALSE(value_array.get_values().empty());
    auto result_1 = std::static_pointer_cast<object::NumberObject>(value_array.get_values()[0]);
    auto result_2 = std::static_pointer_cast<object::NumberObject>(value_array.get_values()[1]);
    EXPECT_EQ(value_array.size(), 2);
    EXPECT_EQ(result_1->value, 1.2);
    EXPECT_EQ(result_2->value, 2.1);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
