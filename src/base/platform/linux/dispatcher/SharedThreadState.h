/*
* Copyright (c) 2015 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef SHRDTHRDST
#define SHRDTHRDST

#include <base/common.h>

#if defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/threading/common/ConditionVariable.h>

using namespace base::threading;

namespace platform
{
	struct SharedThreadState
    {
		pthread_mutex_t blockFDWrites;
		ConditionVariable* blockUntilAHInit;
		int epollFD;
		int fds[2];
    };
} /* platform */

#endif
#endif