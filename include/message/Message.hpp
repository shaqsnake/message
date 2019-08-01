// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:27:32
 * @LastEditTime: 2019-08-01 17:14:54
 * @Description: A declaration of class msg::Message.
 */
#ifndef MESSAGE_MESSAGE_HPP
#define MESSAGE_MESSAGE_HPP

#include <memory>
#include <string>
#include <vector>

namespace msg {

class Message {
public:
    Message() = default;
    ~Message() = default;
    Message(const Message &) = delete;
    Message(Message &&) = delete;
    Message &operator=(const Message &) = delete;
    Message &operator=(Message &&) = delete;

public:
    typedef std::vector<std::pair<std::string, std::string>> Headers;

    bool parseFromMessage(const std::string &rawMessge);
    std::string produceToMessage() const;
    Headers getHeaders() const;
    bool hasHeader(const std::string &headerName) const;
    void setHeader(const std::string &headerName, const std::string &headerValue);
    std::string getHeaderValue(const std::string &headerName) const;
    std::string getBody() const;
    void setBody(const std::string &bodyText);
    void setLineLength(size_t maxLength);

private:
    Message::Headers headers_;
    std::string body_;
    size_t maxLineLength_ = 0;
};

} // namespace msg

#endif // MESSAGE_MESSAGE_HPP
