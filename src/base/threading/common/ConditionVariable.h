/*
* Copyright (c) 2012 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef CONDVAR
#define CONDVAR

#include <base/common.h>
#include <base/base_exports.h>
#include <base/threading/common/thread_types.h>

namespace base { namespace threading
{
	class BASEAPI ConditionVariable
	{
		public:
			ConditionVariable();
			ConditionVariable(PlatformMutex m);
			~ConditionVariable(void) {}
            virtual void Wait();
            virtual void Signal();
		private:
#if defined(OS_WIN)
			#include <windows.h>
			CONDITION_VARIABLE c;
			CRITICAL_SECTION cs;
#else
			pthread_cond_t condition;
			PlatformMutex m;
#endif
	};
}}

#endif
