/*
* Copyright (c) 2015 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef DISPTYPSLINUX
#define DISPTYPSLINUX

#include <base/common.h>

#if !defined(OS_WIN)
#include <functional>
#include <map>

namespace base { namespace threading {
	struct MessageReceiver {
		int filter;
		std::function<void()> recvFunc;
		MessageReceiver(int filter, std::function<void()> func) {
			this->filter = filter;
			this->recvFunc = func;
		}
	};
}} // namespace base::threading

#endif
#endif
