/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MPEPOLL
#define MPEPOLL

#include <mutex>
#include <queue>

#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
namespace platform {
	struct SharedThreadState;
}

namespace base { namespace threading {
	using namespace platform;
	class ConditionVariable;
	class MessagePumpEPoll : public DispatcherMessagePump {
	public:
		MessagePumpEPoll(const char* winIDExt) : DispatcherMessagePump(winIDExt) {}
		void MakeMessagePump(bool isTaskRunner) override;
		void MakeMessagePump(Task* InitTask, bool isTaskRunner) override;
		void StartMessageLoop(bool isTaskRunner) override;
		void PostMessageToThread(const char* thread, Task* task, bool isTaskRunner) override;
		void RegisterMessageFilter(MessageReceiver* recv) override;

	private:
		bool isATaskRunner;
		platform::SharedThreadState* sts;
		std::priority_queue<Task, std::vector<Task>, Task::PriorityComparer> taskQueue;
		static std::map<int, MessageReceiver*> handlers;

		void LockedPopTaskFromQueue();
		void LockedPostTaskToQueue(Task* task);
		platform::SharedThreadState* GetSharedState(bool isTaskRunner);
	};
}} // namespace base::threading

#endif