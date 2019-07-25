// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:27:32
 * @LastEditTime: 2019-07-25 10:01:46
 * @Description:
 */
#ifndef MESSAGE_INTERNETMESSAGE_HPP
#define MESSAGE_INTERNETMESSAGE_HPP

#include <memory>

namespace msg {
class InternetMessage {
public:
    InternetMessage();
    ~InternetMessage();
    InternetMessage(const InternetMessage &) = delete;
    InternetMessage(InternetMessage &&) = delete;
    InternetMessage &operator=(const InternetMessage &) = delete;
    InternetMessage &operator=(InternetMessage &&) = delete;

public:
    int foo();

private:
    struct Impl;
    std::unique_ptr<struct Impl> impl_;
};

} // namespace msg

#endif // MESSAGE_INTERNETMESSAGE_HPP