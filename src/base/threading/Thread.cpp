/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/Utils.h>
#include <base/threading/common/Thread.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
#if defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/platform/linux/dispatcher/SharedThreadState.h>
#endif

using namespace base::utils;

namespace base { namespace threading {
	void Thread::Start(const char *name) {
#if defined(OS_WIN)
		std::string threadWinName;
		threadWinName = "ThreadDispatchWin-";
		threadWinName.append(name);
		this->dispatchWinName = threadWinName.c_str();
		dmp = new MessagePumpWin(this->dispatchWinName);
#elif defined(OS_LINUX) || defined(OS_STEAMLINK)
		this->dispatchWinName = name;
		this->sts = new platform::SharedThreadState();
		dmp = new MessagePumpEPoll(this->dispatchWinName);
#endif
		Dispatcher::Get()->AddNamedThread(this->dispatchWinName, this);

		ThreadID id = PlatformThread::Create(this, this->dispatchWinName);
		if (id == NULL) {
			writeToLog("Thread creation failed.");
		} else {
			this->Id = id;
		}
	}
	void Thread::ThreadMain() {
		NEW_TASK0(InitTask, Thread, this, Init);
		dmp->MakeMessagePump(InitTask, false);
	}
	const char *Thread::GetThreadName() {
		return this->dispatchWinName;
	}
	void Thread::PostTask(DispatcherTask *task) {
		if (dmp == nullptr) { writeToLog("Message pump doesn't even exist. Weird."); }
		dmp->PostMessageToThread(this->dispatchWinName, task, false);
	}
}} // namespace base::threading
