/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MPANDROID
#define MPANDROID

#include <base/threading/dispatcher/DispatcherMessagePump.h>

namespace base { namespace threading 
{
	class MessagePumpAndroid : public DispatcherMessagePump 
	{
		public:
			void PreInitMessagePump();
			void MakeMessagePump(bool isTaskRunner) override;
			void MakeMessagePump(DispatcherTask* InitTask) override;
			void StartMessageLoop(bool isTaskRunner) override;
			void PostMessageToThread(const char* thread, DispatcherTask *task, bool isTaskRunner) override;
			void RegisterMessageHandler(MessageReceiver* recv) override;
		private:
			void GetSharedState(bool isTaskRunner);
	}
}}