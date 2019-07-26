// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:28:37
 * @LastEditTime: 2019-07-26 16:03:46
 * @Description: An implementation of class msg::Http.
 */
#include <algorithm>
#include <iostream>
#include <message/Http.hpp>
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
 * @param[in] str
 *     An input string.
 */
std::string trim(std::string str) {
    ltrim(str);
    rtrim(str);
    return str;
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
    while (start != offset) { // Splite message.
        lines.push_back(rawMessage.substr(start, offset - start));
        start = offset + lineTerminator.size();
        offset = rawMessage.find(lineTerminator, start);
    }
    if (start != rawMessage.length()) // Set body if exists.
        impl_->body = trim(rawMessage.substr(start));

    for (const auto &line :
         lines) { // Extract the name and value filed of headers.
        std::string headerFieldDelimeter = ":";
        auto pos = line.find(headerFieldDelimeter);

        if (pos == 0) // Failed if header's name is blank.
            return false;
        else { // Set headers.
            auto headerName = trim(line.substr(0, pos));
            auto headerValue = trim(line.substr(pos + 1));
            impl_->headers[headerName] = headerValue;
        }
    }

    return true;
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
    return impl_->headers.count(headerName);
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
    return impl_->headers[headerName];
}

std::string Http::getBody() const { return impl_->body; }

} // namespace msg