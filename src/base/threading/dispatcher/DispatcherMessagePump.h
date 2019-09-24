/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef DISPMSGPMP
#define DISPMSGPMP

#include <map>
#include <string>

#include <base/threading/common/ConditionVariable.h>
#include <base/threading/common/thread_types.h>

#if defined(OS_WIN)
#include <base/platform/win/dispatcher/DispatcherTypesWin.h>
#elif defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/platform/linux/dispatcher/DispatcherTypesLinux.h>
#endif

namespace base { namespace threading {
	class Task;

	class BASEAPI DispatcherMessagePump {
	public:
		DispatcherMessagePump() {}
		DispatcherMessagePump(const char* winIDExt) {
			this->startComplete = false;
			this->wndIdExtenstion = winIDExt;
			postBlocker = new ConditionVariable();
			msgPumpWinInit = false;
		}
		~DispatcherMessagePump() {}
		virtual void MakeMessagePump(bool isTaskRunner) = 0;
		virtual void StartMessageLoop(bool isTaskRunner) = 0;
		virtual void MakeMessagePump(Task* InitTask, bool isTaskRunner) = 0;
		virtual void PostMessageToThread(const char* thread, Task* task, bool isTaskRunner) = 0;

		virtual void RegisterMessageFilter(MessageReceiver* recv) = 0;
		const char* wndIdExtenstion;
		bool startComplete;

	protected:
		ConditionVariable* postBlocker;
		static bool msgPumpWinInit;
		Task* initTask;
	};
}} // namespace base::threading

#endif
