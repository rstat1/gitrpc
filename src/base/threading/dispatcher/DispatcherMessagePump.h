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

#if defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/platform/linux/dispatcher/DispatcherTypesLinux.h>
#include <base/platform/linux/dispatcher/SharedThreadState.h>
using namespace platform;
#elif defined(OS_WIN)
#include <base/platform/win/dispatcher/DispatcherTypesWin.h>
#endif

namespace base { namespace threading
{
	class DispatcherTask;

	class BASEAPI DispatcherMessagePump
	{
		public:
			DispatcherMessagePump() {}
			DispatcherMessagePump(const char* winIDExt)
			{
				this->startComplete = false;
				this->wndIdExtenstion = winIDExt;
				postBlocker = new ConditionVariable();
				msgPumpWinInit = false;
			}
			~DispatcherMessagePump() {}
			virtual void MakeMessagePump(bool isTaskRunner) = 0;
			virtual void MakeMessagePump(DispatcherTask* InitTask, bool isTaskRunner) = 0;
			virtual void StartMessageLoop(bool isTaskRunner) = 0;
			virtual void PostMessageToThread(const char* thread, DispatcherTask* task, bool isTaskRunner) = 0;

			//virtual void RegisterMessageHandler(MessageReceiver* recv) = 0;
			const char* wndIdExtenstion;
			bool startComplete;
		protected:
			ConditionVariable* postBlocker;
        	static bool msgPumpWinInit;
			DispatcherTask* initTask;
	};
}}

#endif
