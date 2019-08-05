// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:28:37
 * @LastEditTime: 2019-08-05 14:58:14
 * @Description: An implementation of class msg::Message.
 */
#include <algorithm>
#include <iostream>
#include <message/Message.hpp>
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
// Public methods
/**
 * @description:
 *     Parse a raw Message message into an object,
 *     that contains components of headers, body and etc.
 * @param[in] rawMessage
 *     A raw string in Message message format.
 * @return:
 *     An identicator of whether or not the parse process
 *     was successful is returned.
 */
bool Message::parseFromMessage(const std::string &rawMessage) {
    std::string lineTerminator = "\r\n";
    std::vector<std::string> lines; // Store the splited parts of message.
    std::string::size_type start = 0;
    std::string::size_type offset = rawMessage.find(lineTerminator);
    // Splite message by lines.
    while (start != rawMessage.length()) {
        lines.push_back(rawMessage.substr(start, offset - start));
        start = offset + lineTerminator.size();
        offset = rawMessage.find(lineTerminator, start);

        // Line length exceed the limitation.
        if (offset != std::string::npos && maxLineLength_ &&
            offset - start + 2 > maxLineLength_)
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
                headers_.back().second += line;
            } else { // Set headers.

                auto headerName = line.substr(0, pos);
                // Printable US-ASCII characters except colon
                if (!isValid(headerName, "[\x21-\x39\x3B-\x7E]*"))
                    return false;

                auto headerValue = line.substr(pos + 1);
                // Printable US-ASCII characters and WSP characters
                if (!isValid(headerValue, "[\x9\x20-\x7E]*"))
                    return false;
                headers_.emplace_back(headerName, headerValue);
            }
        } else { // Concat rest message to body.
            body_ += line;
        }
    }

    // Trim whitespace characters of all headers and body field.
    for (auto &header : headers_) {
        trim(header.first);
        trim(header.second);
    }
    if (body_.length())
        trim(body_);

    return true;
}

/**
 * @description:
 *     Produce message components to a complete message.
 * @return:
 *     A message text including headers and body.
 */
std::string Message::produceToMessage() const {
    std::string targetMessage;
    std::vector<std::string> lines;

    dumpToVec(lines);

    // Fold message if set max line length
    if (maxLineLength_) {
        foldMessageLines(lines, maxLineLength_);
    }

    for (const auto &line : lines) {
        targetMessage += line + "\r\n";
    }
    return targetMessage;
}

/**
 * @description:
 *     Get the all headers.
 * @return:
 *     Headers parsed from a raw message.
 */
Message::Headers Message::getHeaders() const { return headers_; }

/**
 * @description:
 *     Check the name of a header is exist or not.
 * @param[in] headerName
 *     the name filed of a header to be checked
 * @return:
 *     An indicator of whether or not the header's name was exist
 *     is returned.
 */
bool Message::hasHeader(const std::string &headerName) const {
    for (const auto &header : headers_) {
        if (header.first == headerName)
            return true;
    }
    return false;
}

/**
 * @description:
 *     Set message header by its name and value.
 * @param[in] headerName
 *     A header's name to identity header.
 * @param[in] headerValue
 *     A heaser's value to be set according to its name.
 * @return:
 */
void Message::setHeader(const std::string &headerName,
                        const std::string &headerValue) {
    for (auto &header : headers_) {
        if (header.first == headerName) {
            header.second = headerValue;
            return;
        }
    }

    headers_.emplace_back(headerName, headerValue);
    return;
}

/**
 * @description:
 *     Get a header's value by its name.
 * @param[in] headerName
 *     A header's name as an index.
 * @return:
 *     The header's value which indexed by its name.
 */
std::string Message::getHeaderValue(const std::string &headerName) const {
    for (const auto &header : headers_) {
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
std::string Message::getBody() const { return body_; }

/**
 * @description:
 *    Set message body by a text.
 * @param[in] bodyText
 *    A text should be set to message body compoent.
 * @return:
 */
void Message::setBody(const std::string &bodyText) { body_ = bodyText; }

/**
 * @description:
 *     Set line length limit number.
 * @param[in] maxLength
 *     A number to limit message each line length.
 */
void Message::setLineLength(size_t maxLength) { maxLineLength_ = maxLength; }

// Private methods
/**
 * @description:
 *     Dump massge headers and body to a container.
 * @param[out] vec
 *     A vector container to store message headers and body.
 */
void Message::dumpToVec(std::vector<std::string> &vec) const {
    for (const auto &header : headers_) {
        vec.push_back(header.first + ": " + header.second);
    }
    vec.push_back("");
    if (!body_.empty()) {
        vec.push_back(body_);
    }
}

/**
 * @description:
 *     Fold message lines by lenght limitation, always break line into segments
 *     at whitespace charater if any.
 * @param[in|out] lines
 *     The lines should be folded.
 * @param[in] maxLength
 *     The max characters limitation per line.
 */
void Message::foldMessageLines(std::vector<std::string> &dest,
                               size_t maxLength) const {
    std::vector<std::string> orig = std::move(dest);
    dest.clear();

    std::string whitespaces = " \t\f\v";
    for (auto &line : orig) {
        if (line == "") { // Keep the blank line.
            dest.push_back(line);
            continue;
        }

        std::string buffer;
        while (line.size()) {
            auto splitPos = line.find_first_of(whitespaces);
            if (splitPos != std::string::npos) { // Splite string by WSP and
                                               // concat to buffer.
                auto offset = line.find_first_not_of(whitespaces, splitPos);
                buffer += line.substr(0, offset);
                line.erase(0, offset);
            } else { // No any WSP in the line.
                buffer += line.substr();
                line.erase();
            }

            // Push every line in limit length.
            if (buffer.length() + 2 > maxLength) {
                auto lastWSP = buffer.find_last_of(whitespaces);
                if (lastWSP != 0) {
                    // Break line at WSP.
                    std::string newLine = buffer.substr(0, lastWSP);
                    rtrim(newLine);
                    dest.push_back(newLine);
                    buffer.erase(0, lastWSP);
                }
            }
        }

        if (buffer.size()) {
            // Keep only one space at the head of a line.
            ltrim(buffer);
            buffer.insert(0, " ");
            dest.push_back(buffer);
        }
    }
}

} // namespace msg
