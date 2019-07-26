// Copyright (c) 2019 shaqsnake. All rights reserved.
/**
 * @Author: shaqsnake
 * @Email: shaqsnake@gmail.com
 * @Date: 2019-07-25 09:28:37
 * @LastEditTime: 2019-07-26 11:29:35
 * @Description: An implementation of class msg::Http.
 */
#include <message/Http.hpp>

namespace msg {
/**
 * The concrete implemetation of Http class.
 */
struct Http::Impl {};

Http::Http() : impl_(new Impl) {}

Http::~Http() noexcept = default;

// Public methods
int Http::foo() {
    return 6 * 7;
}

} // namespace msg