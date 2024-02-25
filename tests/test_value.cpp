#include "value.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(ValueArray, test_write_value) {
    value::ValueArray value_array;
    value_array.write_value(1.2);
    value_array.write_value(2.1);

    EXPECT_THAT(value_array.get_values(), ::testing::ContainerEq(std::vector<double>{1.2, 2.1}));
    EXPECT_EQ(value_array.size(), 2);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}