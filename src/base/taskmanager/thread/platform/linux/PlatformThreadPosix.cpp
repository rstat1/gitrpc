/*
* Copyright (c) 2012-2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <assert.h>
#include <base/taskmanager/thread/platform/common/PlatformThread.h>
#include <sys/syscall.h>

namespace base { namespace taskmanager {
	ThreadID PlatformThread::threadHandle;

	static void* ThreadStartFunction(void* param) {
		ThreadStartInfo* tsi = static_cast<ThreadStartInfo*>(param);
		// Dispatcher* disp = tsi->DispatcherRef;
		PlatformThread::Delegate* del = tsi->ThreadDelegate;
		del->ThreadMain();
		return 0;
	}
	ThreadID PlatformThread::Create(Delegate* delegate, const char* name) {
		ThreadStartInfo* tsi = new ThreadStartInfo;
		tsi->ThreadDelegate = delegate;
		// tsi->DispatcherRef = Dispatcher::Get();
		tsi->threadName = name;

		writeToLog("PlatformThread::Create()");
		if (pthread_create(&PlatformThread::threadHandle, 0, ThreadStartFunction, tsi) == 0) {
			pthread_setname_np(PlatformThread::threadHandle, name);
			return PlatformThread::threadHandle;
		} else {
			writeToLog("pthread_create failed!");
			return NULL;
		}
	}
}} // namespace base::taskmanager