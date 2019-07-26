// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:29:37
 * @LastEditTime: 2019-07-26 15:41:07
 * @Description: Unittests of class msg::Http.
 */
#include <gtest/gtest.h>
#include <message/Http.hpp>
#include <string>
#include <unordered_map>
#include <vector>

TEST(HttpTests, ParseFromHttpRequestString) {
    std::string rawMessage =
        "User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n";
    struct TestCase {
        std::string name;
        std::string value;
    };

    std::vector<TestCase> expectedHeaders{
        {"User-Agent", "curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3"},
        {"Host", "www.example.com"},
        {"Accept-Language", "en, mi"},
    };

    msg::Http http;
    ASSERT_TRUE(http.parseFromMessage(rawMessage));

    auto headers = http.getHeaders();
    ASSERT_EQ(expectedHeaders.size(), headers.size());

    size_t idx = 0;
    for (const auto &expectedHeader : expectedHeaders) {
        ASSERT_TRUE(http.hasHeader(expectedHeader.name));
        ASSERT_EQ(expectedHeader.value,
                  http.getHeaderValue(expectedHeader.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }

    ASSERT_EQ("", http.getBody());
    ASSERT_FALSE(http.hasHeader("spma"));
}