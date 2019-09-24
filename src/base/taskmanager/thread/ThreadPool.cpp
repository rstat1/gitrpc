/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* WARNING: EXPERIMENTAL code. Not intented for production use.
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/taskmanager/dispatcher/TaskManager.h>
#include <base/threading/common/ConditionVariable.h>

namespace base { namespace taskmanager {
	TaskManager::ThreadPool::ThreadPool(int poolSize) {
		newTaskSignal = new base::threading::ConditionVariable();
		for (int i = 0; i < poolSize; i++) {
			auto runnerIter = runners.try_emplace(i, std::make_unique<Runner>(true));
			Runner* runner = (*runnerIter.first).second.get();
			if (runnerIter.second == true) {
				std::string name("TM_THREADPOOL_");
				name.append(std::to_string(i));
				threads.try_emplace(i, std::make_unique<Thread>(runner, name));
				inUseThreads[i] = false;
			} else {
				LOG_MSG("failed to create/insert new task runner, because it already existed");
			}
		}
		queue = std::make_unique<std::priority_queue<Task, std::vector<Task>, Task::PriorityComparer>>();
	}
	void TaskManager::ThreadPool::PostTask(Task* task) {
		std::scoped_lock lock(queueLock);
		LOG_ARGS("posting task %s", task->GetTaskID().c_str())
		queue->push(*task);
	}
	void TaskManager::ThreadPool::ExecuteTasks() {
		int threadID = GetUnusedThread();
		while (!queue->empty()) {
			threadID = GetUnusedThread();
			if (threadID != 255) {
				queueLock.lock();
				Task* t = const_cast<Task*>(&queue->top());
				queueLock.unlock();
				if (!t->IsCompleted()) {
					LOG_ARGS("executing task %s on %i", t->GetTaskID().c_str(), threadID)
					runners[threadID]->SetExecutionStatus();
					runners[threadID]->PostTask(t);
					runners[threadID]->Signal();
				} else {
					LOG_ARGS("task complete(?) %s on %i", t->GetTaskID().c_str(), threadID)
				}
				QueuePop();
			}
		}
	}
	int TaskManager::ThreadPool::GetUnusedThread() {
		for (int i = 0; i < threads.size(); i++) {
			if (!runners[i]->GetExecutionStatus()) {
				return i;
			}
		}
		return 255;
	}
}
} // namespace base::experimental
