/*
* Copyright (c) 2012-2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef THREAD
#define THREAD

#include <base/common.h>
#include <base/threading/common/thread_types.h>
using namespace std;

namespace base { namespace taskmanager {
	class BASEAPI PlatformThread {
	public:
		class BASEAPI Delegate {
		public:
			virtual void ThreadMain() = 0;
			ThreadID Id;
			void* extra;

		protected:
			virtual ~Delegate() {}
		};
		PlatformThread() {}
		~PlatformThread() {}
		PlatformThread(LPVOID param);
		static ThreadID Create(Delegate* delegate, const char* name);
		static ThreadID threadHandle;
	};
	struct ThreadStartInfo {
		PlatformThread::Delegate* ThreadDelegate;
		const char* threadName;
		void* extra;
	};
}} // namespace base::taskmanager

#endif
