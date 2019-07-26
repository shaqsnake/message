// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:27:32
 * @LastEditTime: 2019-07-26 11:29:13
 * @Description: A declaration of class msg::Http.
 */
#ifndef MESSAGE_HTTP_HPP
#define MESSAGE_HTTP_HPP

#include <memory>

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
    int foo();

private:
    struct Impl;
    std::unique_ptr<struct Impl> impl_;
};

} // namespace msg

#endif // MESSAGE_HTTP_HPP