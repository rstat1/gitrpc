/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MPEPOLL
#define MPEPOLL

#include <base/platform/linux/dispatcher/SharedThreadState.h>

namespace base { namespace threading
{
   	class DispatcherTask;

	class MessagePumpEPoll : public DispatcherMessagePump
	{
		public:
			MessagePumpEPoll(const char* winIDExt) : DispatcherMessagePump(winIDExt) {}
			void MakeMessagePump(bool isTaskRunner) override;
			void MakeMessagePump(DispatcherTask* InitTask, bool isTaskRunner) override;
			void StartMessageLoop(bool isTaskRunner) override;
			void PostMessageToThread(const char* thread, DispatcherTask *task, bool isTaskRunner) override;
			void RegisterMessageHandler(MessageReceiver* recv);
		private:
			static std::map<int, MessageReceiver*> handlers;
			SharedThreadState* GetSharedState(bool isTaskRunner);

	};
}}

#endif