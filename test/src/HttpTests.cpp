// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:29:37
 * @LastEditTime: 2019-07-26 11:28:51
 * @Description: Unittests of class msg::Http.
 */
#include <gtest/gtest.h>
#include <message/Http.hpp>

TEST(HttpTests, ParseFromHttpRequestString) {
    msg::Http http;
    ASSERT_EQ(42, http.foo());
}