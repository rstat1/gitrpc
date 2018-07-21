/*
* Copyright (c) 2012-2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <assert.h>
#include <sys/syscall.h>
#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/common/PlatformThread.h>

namespace base { namespace threading
{
	ThreadID PlatformThread::threadHandle;

	static void* ThreadStartFunction(void* param)
	{
		ThreadStartInfo* tsi = static_cast<ThreadStartInfo*>(param);
		Dispatcher* disp = tsi->DispatcherRef;
		PlatformThread::Delegate* del = tsi->ThreadDelegate;
		PlatformThread::SetThreadName(tsi->threadName);
		del->ThreadMain();
		return 0;
	}
	ThreadID CreateThreadInternal(ThreadStartInfo* tsi)
	{
		writeToLog("CreateThreadInternal()");
		if (pthread_create(&PlatformThread::threadHandle, 0, ThreadStartFunction, tsi) == 0)
		{
			return PlatformThread::threadHandle;
		}
		else
		{
			writeToLog("pthread_create failed!");
			return NULL;
		}
	}
	ThreadID PlatformThread::Create(Delegate *delegate, const char *name)
	{
		ThreadStartInfo* tsi = new ThreadStartInfo;
		tsi->ThreadDelegate = delegate;
		tsi->DispatcherRef = Dispatcher::Get();
		tsi->threadName = name;

		writeToLog("PlatformThread::Create()");
		return CreateThreadInternal(tsi);
	}
	void PlatformThread::SetThreadName(const char *name)
	{
		pthread_setname_np(PlatformThread::threadHandle, name);
	}
}}