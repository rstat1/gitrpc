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
	using namespace base::threading;
	TaskManager::Runner::Runner(bool forPoolThread) {
		inUse = ATOMIC_VAR_INIT(false);
		threadPoolTaskRunner = forPoolThread;
		newTaskSignal = new ConditionVariable();
		if (!forPoolThread) {
			queue = std::make_unique<std::priority_queue<Task, std::vector<Task>, Task::PriorityComparer>>();
		}
	}
	void TaskManager::Runner::InitRunnerForThreadPool(ConditionVariable* taskSignal) {
		newTaskSignal = taskSignal;
	}
	void TaskManager::Runner::StartTaskRunner() {
		bool queueEmpty = true;
		while (keepRunning) {
			if (!threadPoolTaskRunner) {
				ExecuteRegularTask();
				if (queue->empty()) {
					newTaskSignal->Wait();
				}
			} else {
				ExecuteThreadPoolTask();
				newTaskSignal->Wait();
			}
		}
	}
	void TaskManager::Runner::ExecuteRegularTask() {
		if (queue->size() > 0) {
			Task* t = const_cast<Task*>(&queue->top());
			if (!t->IsCompleted())
				t->Invoke();
			queue->pop();
		}
	}
	void TaskManager::Runner::ExecuteThreadPoolTask() {
		if (pendingTask.get() != nullptr) {
			pendingTask->Invoke();
			inUse.store(false);
			//	delete pendingTask.release();
		}
	}
	void TaskManager::Runner::PostTask(Task* task) {
		if (!threadPoolTaskRunner) {
			queue->push(*task);
		} else {
			LOG_MSG("set task")
			pendingTask.reset(task);
		}
	}
	void TaskManager::Runner::Signal() {
		newTaskSignal->Signal();
	}
}
} // namespace base::experimental
