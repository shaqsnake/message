// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:29:37
 * @LastEditTime: 2019-08-01 11:07:29
 * @Description: Unittests of class msg::msg.
 */
#include <gtest/gtest.h>
#include <message/Message.hpp>
#include <string>
#include <unordered_map>
#include <vector>

TEST(MessageTests, ParseAndProduceHttpRequestString) {
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

    msg::Message msg;
    ASSERT_TRUE(msg.parseFromMessage(rawMessage));

    auto headers = msg.getHeaders();
    ASSERT_EQ(expectedHeaders.size(), headers.size());

    size_t idx = 0;
    for (const auto &expectedHeader : expectedHeaders) {
        ASSERT_TRUE(msg.hasHeader(expectedHeader.name));
        ASSERT_EQ(expectedHeader.value, msg.getHeaderValue(expectedHeader.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }

    ASSERT_EQ("", msg.getBody());
    ASSERT_FALSE(msg.hasHeader("spma"));
    ASSERT_EQ(rawMessage, msg.produceToMessage());
}

TEST(MessageTests, ParseAndProduceHttpResponseString) {
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

    msg::Message msg;
    ASSERT_TRUE(msg.parseFromMessage(rawMessage));

    auto headers = msg.getHeaders();
    ASSERT_EQ(expectedHeaders.size(), headers.size());

    size_t idx = 0;
    for (const auto &expectedHeader : expectedHeaders) {
        ASSERT_TRUE(msg.hasHeader(expectedHeader.name));
        ASSERT_EQ(expectedHeader.value, msg.getHeaderValue(expectedHeader.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }

    ASSERT_EQ("Hello World! My payload includes a trailing CRLF.",
              msg.getBody());
    ASSERT_FALSE(msg.hasHeader("spma"));
    ASSERT_EQ(rawMessage, msg.produceToMessage());
}

TEST(MessageTests, ParseMessageWithBarlyValidLength) {
    std::string longStr(998, 'x');

    std::vector<std::string> testCases{
        {"Host: www.example.com\r\nX-data: " + longStr.substr(8) + "\r\n\r\n"},
        {"Host: www.example.com\r\n\r\n" + longStr + "\r\n"},
    };

    msg::Message msg;
    size_t idx = 0;
    for (const auto &testCase : testCases) {
        ASSERT_TRUE(msg.parseFromMessage(testCase))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }
}

TEST(MessageTests, ParseMessageWithInvalidLength) {
    std::string longStr(999, 'x');
    std::vector<std::string> testCases{
        {"Host: www.example.com\r\nX-data: " + longStr.substr(8) + "\r\n\r\n"},
        {"Host: www.example.com\r\n\r\n" + longStr + "\r\n"},
    };

    msg::Message msg;
    size_t idx = 0;
    for (const auto &testCase : testCases) {
        ASSERT_FALSE(msg.parseFromMessage(testCase))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }
}

TEST(MessageTests, ParseFromFoldingMessage) {
    struct TestCase {
        std::string rawString;
        msg::Message::Headers expectedHeaders;
        std::string expectedBody;
    };
    std::vector<TestCase> testCases{
        {"Subject: This\r\n is a test\r\n\r\n",
         {
             {"Subject", "This is a test"},
         },
         ""},
        {"Subject: This \r\nis a test\r\n\r\n",
         {
             {"Subject", "This is a test"},
         },
         ""},
        {"Subject: This\r\n is a test \r\n\r\n",
         {
             {"Subject", "This is a test"},
         },
         ""},
        {"Subject: This\r\n is a test \r\nHost: www.exa\r\nmple.com\r\n\r\n",
         {{"Subject", "This is a test"}, {"Host", "www.example.com"}},
         ""},
        {"Host: www.example.com\r\n\r\nI'm \r\nbody.\r\n",
         {
             {"Host", "www.example.com"},
         },
         "I'm body."},
        {"Host: www.\r\nexample.com \r\n\r\n I\r\n'm \r\nbody. \r\n",
         {
             {"Host", "www.example.com"},
         },
         "I'm body."},
    };

    size_t idx = 0;
    for (const auto &testCase : testCases) {
        msg::Message msg;
        ASSERT_TRUE(msg.parseFromMessage(testCase.rawString))
            << ">>> Test is failed at " << idx << ". <<<";
        ASSERT_EQ(testCase.expectedHeaders, msg.getHeaders())
            << ">>> Test is failed at " << idx << ". <<<";
        ASSERT_EQ(testCase.expectedBody, msg.getBody())
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }
}

TEST(MessageTests, ParseFromMessageWithInvaildFormat) {
    std::vector<std::string> testCases{
        "Ho st: www.example.com\r\n\r\n",    "Ho\rst: www.example.com\r\n\r\n",
        "Host: www.ex\rample.com\r\n\r\n",   "Host: www.ex\nample.com\r\n\r\n",
        "Host: www.example.com\x7F\r\n\r\n",
    };

    size_t idx = 0;
    for (const auto &testCase : testCases) {
        msg::Message msg;
        ASSERT_FALSE(msg.parseFromMessage(testCase))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }
}

TEST(MessageTests, ParseFromMessageWithBarelyVaildFormat) {
    std::vector<std::string> testCases{
        "Host: www.ex ample.com\r\n\r\n",
        "Host: www.ex\tample.com\r\n\r\n",
        "Host: www.example.com~\r\n\r\n",
    };

    size_t idx = 0;
    for (const auto &testCase : testCases) {
        msg::Message msg;
        ASSERT_TRUE(msg.parseFromMessage(testCase))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }
}

TEST(MessageTests, SetMessageHeaders) {
    struct TestCase {
        std::string name;
        std::string value;
    };

    std::vector<TestCase> testCases{
        {"Host", "www.example.com"},
        {"X-Data", "XXX"},
        {"Host", "www.foo.com/bar?zoo#spam"},
    };

    std::string expectedMessage = 
        "Host: www.foo.com/bar?zoo#spam\r\n"
        "X-Data: XXX\r\n"
        "\r\n";

    msg::Message msg;
    size_t idx = 0;
    for (const auto &testCase : testCases) {
        msg.setHeader(testCase.name, testCase.value);
        ASSERT_TRUE(msg.hasHeader(testCase.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ASSERT_EQ(testCase.value, msg.getHeaderValue(testCase.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }

    ASSERT_EQ(expectedMessage, msg.produceToMessage());
}
