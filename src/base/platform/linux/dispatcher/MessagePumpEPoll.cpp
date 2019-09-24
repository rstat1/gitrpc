/*
* Copyright (c) 2014-2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/syscall.h>

#include <base/common.h>
#include <base/platform/linux/dispatcher/SharedThreadState.h>
#include <base/threading/common/TaskRunner.h>
#include <base/threading/common/Thread.h>
#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
#include <base/threading/dispatcher/MessagePump.h>

using namespace platform;

namespace base { namespace threading {
	std::map<int, MessageReceiver*> MessagePumpEPoll::handlers;
	bool DispatcherMessagePump::msgPumpWinInit;

	void MessagePumpEPoll::MakeMessagePump(bool isTaskRunner) {
		LOG_ARGS("Current Thread ID: %u, name = %s", syscall(__NR_gettid), this->wndIdExtenstion);
		isATaskRunner = isTaskRunner;
		struct epoll_event eventInfo;
		int epollFD, result;

		LOG_MSG(this->wndIdExtenstion);
		sts = this->GetSharedState(isTaskRunner);

		if (sts == nullptr) {
			LOG_MSG("Why is the null?");
		} else {
			int result = pipe(sts->fds);
			if (result == -1) {
				perror(this->wndIdExtenstion);
				LOG_ARGS("Pipe could create not. Result: %i", errno);
			} else {
				if (fcntl(sts->fds[0], F_SETFL, O_NONBLOCK) != 0) { LOG_MSG("fcntl couldn't. (1)"); }
				if (fcntl(sts->fds[1], F_SETFL, O_NONBLOCK) != 0) { LOG_MSG("fcntl couldn't. (2)"); }

				sts->epollFD = epoll_create(1);
				if (sts->epollFD == -1) { LOG_MSG("Failed to create epoll FD"); }

				memset(&eventInfo, 0, sizeof(epoll_event));
				eventInfo.events = EPOLLIN | EPOLLET;
				eventInfo.data.fd = sts->fds[0];

				result = epoll_ctl(sts->epollFD, EPOLL_CTL_ADD, sts->fds[0], &eventInfo);
				if (result != 0) { LOG_MSG("epoll_ctl failed"); }
			}
		}
		this->StartMessageLoop(isTaskRunner);
	}
	void MessagePumpEPoll::MakeMessagePump(Task* InitTask, bool isTaskRunner) {
		this->initTask = InitTask;
		this->MakeMessagePump(isTaskRunner);
	}
	void MessagePumpEPoll::StartMessageLoop(bool isTaskRunner) {
		LOG_ARGS("Current Thread ID: %u, name = %s", syscall(__NR_gettid), this->wndIdExtenstion)
		const char* n = "\n";
		Task* task;
		int queueSize = 0;
		const char* pipeContent;
		MessageReceiver* recv;
		bool runInitTask = true;
		struct epoll_event events[4];

		if (sts == nullptr) { sts = this->GetSharedState(isTaskRunner); }

		startComplete = true;
		postBlocker->Signal();
		while (1) {
			if (runInitTask && initTask != nullptr) {
				runInitTask = false;
				LOG_MSG("Running init task.")
				PostMessageToThread("", initTask, isTaskRunner);
			}
			epoll_wait(sts->epollFD, events, 4, -1);
			for (int i = 0; i < 4; i++) {
				if (events[i].data.fd > 0) {
					//This will BLOCK the thread for duration of hook function.
					auto it = handlers.find(events[i].data.fd);
					if (it != handlers.end()) {
						recv = it->second;
						recv->recvFunc();
					}
				}
			}
			if (read(sts->fds[0], &pipeContent, strlen(n)) == -1) {
				perror(this->wndIdExtenstion);
				LOG_ARGS("Pipe read failed Result: %i", errno);
			}
			sts->queueGuard.lock();
			while (taskQueue.size() > 0) {
				auto task = std::move(taskQueue.top());
				taskQueue.pop();
				if (!task.IsComplete()) {
					task.Invoke(!task.HasArguments());
				}
			}
			sts->queueGuard.unlock();
		}
	}
	void MessagePumpEPoll::LockedPopTaskFromQueue() {
	}
	void MessagePumpEPoll::LockedPostTaskToQueue(Task* task) {
		std::lock_guard<std::mutex> lock(sts->queueGuard);

		if (sts == nullptr) { sts = this->GetSharedState(isATaskRunner); }
		taskQueue.push(std::move(*task));
	}
	void MessagePumpEPoll::PostMessageToThread(const char* thread, Task* task, bool isTaskRunner) {
		const char* n = "\n";
		if (startComplete == false) { this->postBlocker->Wait(); }
		LockedPostTaskToQueue(std::move(task));
		if (write(sts->fds[1], n, strlen(n)) == -1) {
			perror(this->wndIdExtenstion);
			LOG_ARGS("Pipe write failed Result: %i", errno);
		}
	}
	void MessagePumpEPoll::RegisterMessageFilter(MessageReceiver* recv) {
		struct epoll_event eventInfo;
		handlers[recv->filter] = recv;
		memset(&eventInfo, 0, sizeof(epoll_event));
		eventInfo.events = EPOLLIN | EPOLLET;
		eventInfo.data.fd = recv->filter;
		epoll_ctl(sts->epollFD, EPOLL_CTL_ADD, recv->filter, &eventInfo);
	}
	SharedThreadState* MessagePumpEPoll::GetSharedState(bool isTaskRunner) {
		if (isTaskRunner) {
			TaskRunner* taskRunner = Dispatcher::Get()->GetTaskRunner(this->wndIdExtenstion);
			sts = (SharedThreadState*)taskRunner->extra;
		} else {
			return Dispatcher::Get()->GetThread(this->wndIdExtenstion)->sts;
		}
		if (sts == nullptr) {
			return nullptr;
		} else {
			return sts;
		}
	}
}} // namespace base::threading