// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:29:37
 * @LastEditTime: 2019-07-27 17:04:35
 * @Description: Unittests of class msg::Http.
 */
#include <gtest/gtest.h>
#include <message/Http.hpp>
#include <string>
#include <unordered_map>
#include <vector>

TEST(HttpTests, ParseAndProduceHttpRequestString) {
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
    ASSERT_EQ(rawMessage, http.produceToMessage());
}

TEST(HttpTests, ParseAndProduceHttpResponseString) {
    std::string rawMessage =
        "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
        "Server: Apache\r\n"
        "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
        "ETag: \"34aa387-d-1568eb00\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 51\r\n"
        "Vary: Accept-Encoding\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello World! My payload includes a trailing CRLF.\r\n";

    struct TestCase {
        std::string name;
        std::string value;
    };

    std::vector<TestCase> expectedHeaders{
        {"Date", "Mon, 27 Jul 2009 12:28:53 GMT"},
        {"Server", "Apache"},
        {"Last-Modified", "Wed, 22 Jul 2009 19:15:56 GMT"},
        {"ETag", "\"34aa387-d-1568eb00\""},
        {"Accept-Ranges", "bytes"},
        {"Content-Length", "51"},
        {"Vary", "Accept-Encoding"},
        {"Content-Type", "text/plain"},
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

    ASSERT_EQ("Hello World! My payload includes a trailing CRLF.", http.getBody());
    ASSERT_FALSE(http.hasHeader("spma"));
    ASSERT_EQ(rawMessage, http.produceToMessage());
}