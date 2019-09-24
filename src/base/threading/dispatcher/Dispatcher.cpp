/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#define INCLUDE_DEFINTIONS 1

#include <assert.h>
#include <base/threading/common/TaskRunner.h>
#include <base/threading/common/Thread.h>
#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
#if defined(OS_WIN)
#include <windows.h>
#else
#include <sys/syscall.h>
#endif

using namespace base::utils;

namespace base { namespace threading {
	std::shared_ptr<Dispatcher> Dispatcher::ref;

	Dispatcher::Dispatcher() {}
	Dispatcher* Dispatcher::Get() {
		if (!Dispatcher::ref) { ref = make_shared<Dispatcher>(); }
		return ref.get();
	}
	void Dispatcher::AddMessagePump(const char* name, DispatcherMessagePump* dmp, int parentThreadID) {
		LOG_ARGS("AddMessagePump %s, with ID %i", name, parentThreadID);
		if (this->knownDMPs.find(name) == knownDMPs.end()) {
			this->knownDMPs[name] = dmp;
			this->threadIDs[parentThreadID] = name;
		} else {
			LOG_ARGS("Not re-adding existing DMP named: %s", name)
		}
	}
	void Dispatcher::AddTaskRunner(const char* name, TaskRunner* dmpRef) {
		LOG_ARGS("AddTaskRunner %s, with ID %i", name, dmpRef->Id);
		if (this->namedDMPs.find(name) == namedDMPs.end()) {
			this->namedDMPs[name] = dmpRef;
			this->threadIDs[dmpRef->Id] = name;
		} else {
			LOG_ARGS("Not re-adding existing task runner named: %s", name)
		}
	}
	void Dispatcher::AddNamedThread(const char* name, Thread* threadRef) {
		LOG_ARGS("AddNamedThread %s, with ID %i", name, threadRef->Id);
		if (this->namedThreads.find(name) == namedThreads.end()) {
			this->namedThreads[name] = threadRef;
			this->threadIDs[threadRef->Id] = name;
		} else {
			LOG_ARGS("Not re-adding existing thread named: %s", name)
		}
	}
	void Dispatcher::PostTaskToTaskRunner(Task* taskToRun, const char* DispatcherName) {
		TaskRunner* runner = GetTaskRunner(DispatcherName);
		if (runner != nullptr) {
			runner->RunTask(taskToRun);
		} else {
			LOG_ARGS("Couldn't find task runner named %s", DispatcherName)
		}
	}
	void Dispatcher::PostTaskToThread(Task* TaskToComplete, const char* threadName) {
		Thread* ref = GetThread(threadName);
		if (ref != nullptr) {
			ref->PostTask(TaskToComplete);
		} else {
			this->PostTaskToTaskRunner(TaskToComplete, threadName);
		}
	}
	void Dispatcher::PostTask(const char* receiver, Task* task) {
		Thread* ref = GetThread(receiver);
		TaskRunner* runner = GetTaskRunner(receiver);
		DispatcherMessagePump* messagePump = GetMessagePump(receiver);

		if (runner != nullptr) {
			runner->RunTask(task);
		} else if (ref != nullptr) {
			ref->PostTask(task);
		} else if (messagePump != nullptr) {
			messagePump->PostMessageToThread(receiver, task, false);
		} else {
			LOG_ARGS("unknown thread/message pump/task runner %s", receiver)
		}
	}
	TaskRunner* Dispatcher::GetTaskRunner(const char* name) {
		auto mapIter = this->namedDMPs.find(name);
		if (mapIter != namedDMPs.end()) {
			return mapIter->second;
		} else {
			return nullptr;
		}
	}
	Thread* Dispatcher::GetThread(const char* name) {
		auto mapIter = this->namedThreads.find(name);
		if (mapIter != namedThreads.end()) {
			return mapIter->second;
		} else {
			return nullptr;
		}
	}
	DispatcherMessagePump* Dispatcher::GetMessagePump(const char* name) {
		auto mapIter = this->knownDMPs.find(name);
		if (mapIter != knownDMPs.end()) {
			return mapIter->second;
		} else {
			return nullptr;
		}
	}
	ThreadID Dispatcher::GetThreadId(const char* name) {
		Thread* thread = GetThread(name);
		TaskRunner* taskRunner = GetTaskRunner(name);

		if (taskRunner != nullptr) {
			return taskRunner->Id;
		} else if (thread != nullptr) {
			return thread->Id;
		} else {
			LOG_ARGS("unknown thread/task runner, or name belongs to a raw DMP. %s", name)
		}
		return 0;
	}
	int Dispatcher::GetCurrentThreadId() {
		return GetThreadID();
	}
	const char* Dispatcher::GetThreadNameFromId(int id) {
		auto mapIter = this->threadIDs.find(id);
		if (mapIter != threadIDs.end()) {
			return mapIter->second;
		} else {
			return "";
		}
	}
	bool Dispatcher::IsCorrectThread(const char* correctThread) {
#if !defined(OS_WIN)
		const char* currentThreadName = this->GetThreadNameFromId(GetPthreadID());
		LOG_ARGS("thread check -- correctThread %s, currentthread: %s",
				 correctThread, currentThreadName)
		return pthread_equal(this->GetThreadId(correctThread), GetPthreadID()) != 0;
#else
		NOTIMPLEMENTED();
		//TODO: Not-POSIX thread checking...
		return true;
		//return strcmp(currentThreadName, correctThread) == 0;
#endif
	}
}} // namespace base::threading