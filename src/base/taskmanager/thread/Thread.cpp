/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* WARNING: EXPERIMENTAL code. Not intented for production use.
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>

#include <base/taskmanager/dispatcher/TaskManager.h>

namespace base { namespace taskmanager {
	TaskManager::Thread::Thread(Runner* runner, std::string name) {
		taskRunner = runner;
		ThreadID id = PlatformThread::Create(this, name.c_str());
		if (id == NULL) {
			LOG_MSG("failed to create thread!")
		}
	}
	void TaskManager::Thread::StartDispatchLoop() {
		taskRunner->StartTaskRunner();
	}
	void TaskManager::Thread::ThreadMain() {
		LOG_MSG("on thread")
		StartDispatchLoop();
	}
}} // namespace base::taskmanager