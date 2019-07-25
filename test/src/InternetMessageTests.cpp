// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:29:37
 * @LastEditTime: 2019-07-25 10:03:00
 * @Description: 
 */
#include <gtest/gtest.h>
#include <message/InternetMessage.hpp>

TEST(InternetMessageTests, Placeholder) {
    msg::InternetMessage internetMessage;
    ASSERT_EQ(42, internetMessage.foo());
}