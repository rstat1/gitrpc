/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MSGLOOP
#define MSGLOOP

#include <functional>
#include <map>
#include <memory>

#include <base/Utils.h>
#include <base/threading/common/thread_types.h>

namespace base { namespace threading {
	class Thread;
	class TaskRunner;
	class Task;
	class DispatcherMessagePump;

	class BASEAPI Dispatcher {
	public:
		Dispatcher();
		static Dispatcher* Get();
		const char* GetThreadNameFromId(int threadId);
		void AddMessagePump(const char* name, DispatcherMessagePump* dmp, int parentThreadID);
		void AddTaskRunner(const char* name, TaskRunner* dmpRef);
		void AddNamedThread(const char* name, Thread* threadRef);
		void PostTask(const char* receiver, Task* task);
		void PostTaskToTaskRunner(Task* taskToRun, const char* DispatcherName);
		void PostTaskToThread(Task* TaskToComplete, const char* threadName);
		Thread* GetThread(const char* name);
		TaskRunner* GetTaskRunner(const char* name);
		DispatcherMessagePump* GetMessagePump(const char* name);
		int GetCurrentThreadId();
		ThreadID GetThreadId(const char* name);
		bool IsCorrectThread(const char* correctThread);

	private:
		static std::shared_ptr<Dispatcher> ref;
		std::map<const char*, DispatcherMessagePump*, base::utils::CStringComparator> knownDMPs;
		std::map<const char*, Thread*, base::utils::CStringComparator> namedThreads;
		std::map<const char*, TaskRunner*, base::utils::CStringComparator> namedDMPs;
		std::map<ThreadID, const char*> threadIDs;
	};
}} // namespace base::threading

#endif