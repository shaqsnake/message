// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:29:37
 * @LastEditTime: 2019-08-15 17:11:42
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
    msg.setLineLength(1000);
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
    msg.setLineLength(1000);
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
        {"Subject: This\r\n is a test\r\n\r\n", {{"Subject", "This is a test"},}, ""},
        {"Subject: This\r\n is a test \r\n\r\n", {{"Subject", "This is a test"},}, ""},
        {"Host: www.example.com\r\n\r\nI'm \r\nbody.\r\n", {{"Host", "www.example.com"},}, "I'm body."},
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

TEST(MessageTests, SetMessageHeadersAndBody) {
    struct TestCase {
        std::string name;
        std::string value;
    };

    std::vector<TestCase> testCases{
        {"Host", "www.example.com"},
        {"X-Data", "XXX"},
        {"Host", "www.foo.com/bar?zoo#spam"},
    };

    std::string bodyText = "I'm body!";
    std::string expectedMessage = "Host: www.foo.com/bar?zoo#spam\r\n"
                                  "X-Data: XXX\r\n"
                                  "\r\n"
                                  "I'm body!\r\n";

    msg::Message msg;
    size_t idx = 0;
    for (const auto &testCase : testCases) {
        msg.setHeader(testCase.name, testCase.value, true);
        ASSERT_TRUE(msg.hasHeader(testCase.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ASSERT_EQ(testCase.value, msg.getHeaderValue(testCase.name))
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }

    msg.setBody(bodyText);
    ASSERT_EQ(bodyText, msg.getBody());
    ASSERT_EQ(expectedMessage, msg.produceToMessage());
}

TEST(MessageTests, ProduceToMessageByFolding) {
    struct TestCase {
        std::string rawMessage;
        size_t maxLength;
        std::string expectMessage;
    };

    std::vector<TestCase> testCases {
        {"Subject: This is a test\r\n\r\n", 0, "Subject: This is a test\r\n\r\n"},
        {"Subject: This  is a test\r\n\r\n", 14, "Subject: This\r\n is a test\r\n\r\n"},
        {"Subject: This is a test of parsing message.\r\n\r\n", 14, "Subject: This\r\n is a test of\r\n parsing\r\n message.\r\n\r\n"},
        {"Subject: This is a test\r\n\r\n", 14, "Subject: This\r\n is a test\r\n\r\n"},
        {"Subject: This \tis a test\r\n\r\n", 14, "Subject: This\r\n is a test\r\n\r\n"},
        {"Subject: This\tis\ta\ttest\r\n\r\n", 14, "Subject: This\r\n is\ta\ttest\r\n\r\n"},
        {"Subject: This is a test\r\n\r\n", 15, "Subject: This\r\n is a test\r\n\r\n"},
        {"Subject: This is a test\r\n\r\n", 16, "Subject: This is\r\n a test\r\n\r\n"},
        {"Subject: This is a test\r\n\r\n", 20, "Subject: This is a\r\n test\r\n\r\n"},
        {"Subject: This is a test\r\n\r\n", 10, "Subject:\r\n This is\r\n a test\r\n\r\n"},
        {"Subject: This is a test\r\n\r\n", 5, "Subject:\r\n This\r\n is\r\n a\r\n test\r\n\r\n"},
        {"Subject: This is a test\r\nX-Data: xxxxxx xxx\r\n\r\n", 5, "Subject:\r\n This\r\n is\r\n a\r\n test\r\nX-Data:\r\n xxxxxx\r\n xxx\r\n\r\n"},
        {"Subject: This is a test\r\nX-Data: xxxxxx xxx\r\n\r\nI'm\tbody!!!\r\n", 5, "Subject:\r\n This\r\n is\r\n a\r\n test\r\nX-Data:\r\n xxxxxx\r\n xxx\r\n\r\nI'm\r\n body!!!\r\n"},
    };

    size_t idx = 0;
    for (const auto &testCase : testCases) {
        msg::Message msg;
        ASSERT_TRUE(msg.parseFromMessage(testCase.rawMessage))
            << ">>> Test is failed at " << idx << ". <<<";
        ASSERT_EQ(testCase.rawMessage, msg.produceToMessage())
            << ">>> Test is failed at " << idx << ". <<<";
        if (testCase.maxLength) msg.setLineLength(testCase.maxLength);
        ASSERT_EQ(testCase.expectMessage, msg.produceToMessage())
            << ">>> Test is failed at " << idx << ". <<<";
        ++idx;
    }
}

TEST(MessageTests, GetMultiValuesByHeaderName) {
    std::string rawMessage =
        "Via: SIP/2.0/UDP server10.biloxi.com\r\n"
        "    ;branch=z9hG4bKnashds8;received=192.0.2.3\r\n"
        "Via: SIP/2.0/UDP bigbox3.site3.atlanta.com\r\n"
        "    ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2\r\n"
        "Via: SIP/2.0/UDP pc33.atlanta.com\r\n"
        "    ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
        "To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
        "\r\n";

    msg::Message msg;
    ASSERT_TRUE(msg.parseFromMessage(rawMessage));
    ASSERT_TRUE(msg.hasHeader("Via"));
    ASSERT_EQ(
        "SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3, "
        "SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2, "
        "SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1",
        msg.getHeaderValue("Via")
    );
}

TEST(MessageTests, RemoveHeaderByName) {
    std::string rawMessage = 
        "From: John Doe <jdoe@machine.example>\r\n"
        "To: Mary Smith <mary@example.net>\r\n"
        "Subject: Saying Hello\r\n"
        "Date: Fri, 21 Nov 1997 09:55:06 -0600\r\n"
        "Message-ID: <1234@local.machine.example>\r\n"
        "\r\n"
        "This is a message just to say hello.\r\n"
        "So, \"Hello\".\r\n";

    msg::Message msg;
    ASSERT_TRUE(msg.parseFromMessage(rawMessage));
    ASSERT_TRUE(msg.hasHeader("Message-ID"));
    msg.removeHeader("Message-ID");
    ASSERT_FALSE(msg.hasHeader("Message-ID"));

    ASSERT_TRUE(msg.hasHeader("Date"));
    msg.removeHeader("Date");
    ASSERT_FALSE(msg.hasHeader("Date"));
}