/*
* Copyright (c) 2014 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef DISPTYPSWIN
#define DISPTYPSWIN

#include <base/common.h>

#if defined(OS_WIN)
#include <functional>
#include <Windows.h>

namespace base { namespace threading
{
	struct MessageReceiver
	{
		HWND filter;
		std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> recvFunc;
		MessageReceiver(HWND filter, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> func) { this->filter = filter; this->recvFunc = func; }
	};
}}

#endif
#endif
