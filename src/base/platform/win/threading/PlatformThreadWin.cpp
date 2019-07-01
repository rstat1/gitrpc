/*
* Copyright (c) 2014 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <assert.h>
#include <base/threading/common/PlatformThread.h>
#include <base/threading/dispatcher/Dispatcher.h>

namespace base { namespace threading {
	ThreadID PlatformThread::threadHandle;

	DWORD __stdcall ThreadStartFunction(void *param) {

		ThreadStartInfo *tsi = static_cast<ThreadStartInfo *>(param);
		PlatformThread::Delegate *del = tsi->ThreadDelegate;
		Dispatcher *disp = tsi->DispatcherRef;
		PlatformThread::SetThreadName(tsi->threadName);

		tsi->ThreadDelegate->Id = 0;
		del->ThreadMain();
		return 0;
	}
	ThreadID CreateThreadInternal(ThreadStartInfo *tsi) {
		HANDLE thread = CreateThread(NULL, 0, ThreadStartFunction, tsi, 0, &PlatformThread::threadHandle);
		assert(thread != NULL);
		return PlatformThread::threadHandle;
	}
	ThreadID PlatformThread::Create(PlatformThread::Delegate *delegate, const char *name) {
		ThreadStartInfo *tsi = new ThreadStartInfo;
		tsi->ThreadDelegate = delegate;
		tsi->DispatcherRef = Dispatcher::Get();
		tsi->threadName = name;

		return CreateThreadInternal(tsi);
	}
	void PlatformThread::SetThreadName(const char *name) {
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = name;
		info.dwThreadID = PlatformThread::threadHandle;
		info.dwFlags = 0;

		__try {
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), reinterpret_cast<DWORD_PTR *>(&info));
		} __except (EXCEPTION_CONTINUE_EXECUTION) {}
	}
}} // namespace base::threading