/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef IPCTYPS
#define IPCTYPS

#include <base/common.h>

#if defined(OS_WIN)
#include <windows.h>
typedef HANDLE PlatformHandle;
#elif defined(OS_LINUX)
typedef int PlatformHandle;
#endif

struct ChannelInfo
{
	public:
		int epollFD;
		int pipeFDs[2];
};

#endif