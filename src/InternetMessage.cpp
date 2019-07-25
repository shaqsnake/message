// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:28:37
 * @LastEditTime: 2019-07-25 10:02:22
 * @Description:
 */
#include <message/InternetMessage.hpp>

namespace msg {
/**
 * The concrete implemetation of InternetMessage class.
 */
struct InternetMessage::Impl {};

InternetMessage::InternetMessage() : impl_(new Impl) {}

InternetMessage::~InternetMessage() noexcept = default;

// Public methods
int InternetMessage::foo() {
    return 6 * 7;
}

} // namespace msg