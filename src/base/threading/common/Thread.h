/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MSGLOOPTHRD
#define MSGLOOPTHRD

#include <base/threading/common/PlatformThread.h>
#include <base/threading/dispatcher/MessagePump.h>

namespace base { namespace threading {
	class Task;
#if defined(OS_WIN)
	const DWORD MS_VC_EXCEPTION = 0x406D1388;
	typedef struct tagTHREADNAME_INFO {
		DWORD dwType;
		LPCSTR szName;
		DWORD dwThreadID;
		DWORD dwFlags;
	} THREADNAME_INFO;
#endif

	class BASEAPI Thread : public PlatformThread::Delegate {
	public:
		Thread() {}
		Thread(const char* threadName) { this->dispatchWinName = threadName; }
		void Start(const char* name) override;
		void PostTask(Task* task);
		void ThreadMain() override;
		const char* GetThreadName();
		DispatcherMessagePump* dmp;

	protected:
		virtual void Init() = 0;
		const char* dispatchWinName;

	private:
	};

}} // namespace base::threading

#endif
