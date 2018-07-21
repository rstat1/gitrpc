/*
* Copyright (c) 2012-2013 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef THREAD
#define THREAD

#include <base/common.h>
#include <base/base_exports.h>
#include <base/threading/common/thread_types.h>
#if defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/platform/linux/dispatcher/SharedThreadState.h>
using namespace platform;
#endif
using namespace std;

namespace base { namespace threading
{
	class Dispatcher;

    class BASEAPI PlatformThread
	{
		public:
			class BASEAPI Delegate
			{
				public:
					virtual void Start(const char* name) = 0;
					virtual void ThreadMain() = 0;
					ThreadID Id;
					#if defined(OS_LINUX) || defined(OS_STEAMLINK)
					SharedThreadState* sts;
					#endif
					void* extra;
				protected:
					virtual ~Delegate() {}
			};
			PlatformThread() { }
			~PlatformThread() {}
			PlatformThread(LPVOID param);
			static ThreadID Create(Delegate* delegate, const char* name);
			static void SetThreadName(const char* name);
			static ThreadID threadHandle;

	};
	struct ThreadStartInfo
	{
		PlatformThread::Delegate* ThreadDelegate;
		Dispatcher* DispatcherRef;
		const char* threadName;
		void* extra;
	};
}}

//#if defined(OS_WIN)
//#include <base/threading/platform/PlatformThreadWin.h>
//#else
//#include <base/threading/platform/PlatformThreadPosix.h>
//#endif

#endif
