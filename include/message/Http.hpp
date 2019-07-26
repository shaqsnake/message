// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:27:32
 * @LastEditTime: 2019-07-26 15:46:48
 * @Description: A declaration of class msg::Http.
 */
#ifndef MESSAGE_HTTP_HPP
#define MESSAGE_HTTP_HPP

#include <memory>
#include <string>
#include <unordered_map>

namespace msg {

class Http {
public:
    Http();
    ~Http();
    Http(const Http &) = delete;
    Http(Http &&) = delete;
    Http &operator=(const Http &) = delete;
    Http &operator=(Http &&) = delete;

public:
    typedef std::unordered_map<std::string, std::string> Headers;

    bool parseFromMessage(const std::string &rawMessge);
    Headers getHeaders() const;
    bool hasHeader(const std::string &headerName) const;
    std::string getHeaderValue(const std::string &headerName) const;
    std::string getBody() const;

private:
    struct Impl;
    std::unique_ptr<struct Impl> impl_;
};

} // namespace msg

#endif // MESSAGE_HTTP_HPP