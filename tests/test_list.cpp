#include "list.h"
#include "object.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ds::List;
using ds::Node;
using object::StringObject;

TEST(ListTest, empty) {
    List<int> list{};
    EXPECT_TRUE(list.empty());
}

TEST(ListTest, insert) {
    auto obj = new StringObject{"hi"};
    List<StringObject> str_list;
    str_list.insert(obj);

    EXPECT_FALSE(str_list.empty());
}

TEST(ListTest, list_iterator) {
    auto obj_1 = new StringObject{"hi"};
    auto obj_2 = new StringObject{"there!"};
    auto obj_3 = new StringObject{"My name is Zafer."};

    List<StringObject> str_list;
    str_list.insert(obj_1);
    str_list.insert(obj_2);
    str_list.insert(obj_3);

    EXPECT_EQ(str_list.size(), 3);

    auto itr = str_list.begin();
    EXPECT_EQ(itr->item->to_string(), "My name is Zafer.");
    ++itr;
    EXPECT_EQ(itr->item->to_string(), "there!");
    ++itr;
    EXPECT_EQ(itr->item->to_string(), "hi");
}

TEST(ListTest, erase_if) {
    auto obj_1 = new StringObject{"hi"};
    auto obj_2 = new StringObject{"there!"};
    auto obj_3 = new StringObject{"My name is Zafer."};

    List<StringObject> str_list;
    str_list.insert(obj_1);
    str_list.insert(obj_2);
    str_list.insert(obj_3);

    auto pred = [](Node<StringObject>* node) {
        if (node->item->to_string() == "there!") {
            return true;
        }
        return false;
    };

    str_list.erase_if(pred);
    EXPECT_FALSE(str_list.empty());
    EXPECT_EQ(str_list.size(), 2);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
