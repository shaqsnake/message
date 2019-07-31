// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:28:37
 * @LastEditTime: 2019-07-31 09:42:45
 * @Description: An implementation of class msg::Http.
 */
#include <algorithm>
#include <iostream>
#include <message/Http.hpp>
#include <regex>
#include <vector>

namespace {
/**
 * @description:
 *     Remove the whitespace charaters
 *     at the left of an input string.
 * @param[in] s
 *     An input string.
 */
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));
}

/**
 * @description:
 *     Remove the whitespace characters
 *     at the right of an input string.
 * @param[in] s
 *     An input string.
 */
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

/**
 * @description:
 *     Remove the whitspace characters
 *     at the both sides of an input string.
 * @param[in] s
 *     An input string.
 */
void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

/**
 * @description:
 *     Check a string matches the pattern.
 * @param[in] s
 *     A string to be checked.
 * @param[in] pattern
 *     A pattern to check.
 * @return:
 *     An indicator whether or not was valid is return.
 */
bool isValid(const std::string &s, std::string pattern) {
    return std::regex_match(s, std::regex(pattern));
}

} // namespace

namespace msg {
/**
 * The concrete implemetation of Http class.
 */
struct Http::Impl {
    Http::Headers headers;
    std::string body;
};

/**
 * Constructor.
 */
Http::Http() : impl_(new Impl) {}

/**
 * Destructor by default.
 */
Http::~Http() noexcept = default;

// Public methods
/**
 * @description:
 *     Parse a raw http message into an object,
 *     that contains components of headers, body and etc.
 * @param[in] rawMessage
 *     A raw string in http message format.
 * @return:
 *     An identicator of whether or not the parse process
 *     was successful is returned.
 */
bool Http::parseFromMessage(const std::string &rawMessage) {
    std::string lineTerminator = "\r\n";
    std::vector<std::string> lines; // Store the splited parts of message.
    std::string::size_type start = 0;
    std::string::size_type offset = rawMessage.find(lineTerminator);
    // Splite message by lines.
    while (start != rawMessage.length()) {
        lines.push_back(rawMessage.substr(start, offset - start));
        start = offset + lineTerminator.size();
        offset = rawMessage.find(lineTerminator, start);

        if (offset != std::string::npos && offset - start > 998)
            return false;
    }

    // Handle each line and assign it to header or body.
    bool doneWithHeaders = false;
    for (const auto &line : lines) {
        if (line == "") {
            doneWithHeaders = true;
            continue;
        }

        if (!doneWithHeaders) { // Extract the name and value filed of headers.
            std::string headerFieldDelimeter = ":";

            auto pos = line.find(headerFieldDelimeter);
            if (pos == std::string::npos) {
                impl_->headers.back().second += line;
            } else { // Set headers.

                auto headerName = line.substr(0, pos);
                // Printable US-ASCII characters except colon
                if (!isValid(headerName, "[\x21-\x39\x3B-\x7E]*"))
                    return false;

                auto headerValue = line.substr(pos + 1);
                // Printable US-ASCII characters and WSP characters
                if (!isValid(headerValue, "[\x9\x20-\x7E]*"))
                    return false;
                impl_->headers.emplace_back(headerName, headerValue);
            }
        } else { // Concat rest message to body.
            impl_->body += line;
        }
    }

    // Trim whitespace characters of all headers and body field.
    for (auto &header : impl_->headers) {
        trim(header.first);
        trim(header.second);
    }
    if (impl_->body.length())
        trim(impl_->body);

    return true;
}

/**
 * @description:
 *     Produce http components to a complete http message.
 * @return:
 *     A http message including headers and body.
 */
std::string Http::produceToMessage() const {
    std::string targetMessage;
    for (const auto &header : impl_->headers) {
        targetMessage += header.first + ": " + header.second + "\r\n";
    }

    targetMessage += "\r\n";
    if (!impl_->body.empty())
        targetMessage += impl_->body + "\r\n";

    return targetMessage;
}

/**
 * @description:
 *     Get the all headers.
 * @return:
 *     Headers parsed from a raw http message.
 */
Http::Headers Http::getHeaders() const { return impl_->headers; }

/**
 * @description:
 *     Check the name of a header is exist or not.
 * @param[in] headerName
 *     the name filed of a header to be checked
 * @return:
 *     An indicator of whether or not the header's name was exist
 *     is returned.
 */
bool Http::hasHeader(const std::string &headerName) const {
    for (const auto &header : impl_->headers) {
        if (header.first == headerName)
            return true;
    }
    return false;
}

/**
 * @description:
 *     Get a header's value by its name.
 * @param[in] headerName
 *     A header's name as an index.
 * @return:
 *     The header's value which indexed by its name.
 */
std::string Http::getHeaderValue(const std::string &headerName) const {
    for (const auto &header : impl_->headers) {
        if (header.first == headerName)
            return header.second;
    }
    return "";
}

/**
 * @description:
 *     Get message body text.
 * @return:
 *     A text of message body.
 */
std::string Http::getBody() const { return impl_->body; }

} // namespace msg
