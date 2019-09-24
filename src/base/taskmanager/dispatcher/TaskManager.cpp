/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* WARNING: EXPERIMENTAL code. Not intented for production use.
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/taskmanager/dispatcher/TaskManager.h>
#include <thread>

namespace base { namespace taskmanager {
	SINGLETON_DEF(TaskManager);
	void TaskManager::InitializeThreadPool(int threadPoolSize) {
		if (threadPoolSize > std::thread::hardware_concurrency() || threadPoolSize == -1) {
			threadPoolSize = std::thread::hardware_concurrency();
		}
		if (threadPoolSize > 0) {
			threadPool = new TaskManager::ThreadPool(threadPoolSize);
		}
	}
	void TaskManager::InitializeOnCurrentThread(std::string queueName, Task* initTask) {
		runners.try_emplace(queueName, std::make_unique<Runner>(false));
		runners.at(queueName)->PostTask(initTask);
		runners.at(queueName)->StartTaskRunner();
	}
	void TaskManager::PostNonThreadPoolTask(Task* task, std::string queueName) {
		Runner* runner = runners.at(queueName).get();
		if (runner != nullptr) {
			runner->PostTask(task);
			runner->Signal();
		}
	}
	void TaskManager::PostThreadPoolTask(Task* task) {
		//		LOG_MSG("posting task to thread pool")
		threadPool->PostTask(task);
		threadPool->ExecuteTasks();
	}
	void TaskManager::CreateRunnerOnNewThread(std::string name, Task* initTask) {
		auto runnerIter = runners.try_emplace(name, std::make_unique<Runner>(false));
		if (runnerIter.second == true) {
			threads.try_emplace(name, std::make_unique<Thread>((*runnerIter.first).second.get(), "TM_THREAD"));
		} else {
			LOG_MSG("failed to create/insert new task runner, because it already existed");
		}
	}
}} // namespace base::experimental
