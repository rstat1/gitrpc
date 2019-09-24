/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* WARNING: EXPERIMENTAL code. Not intented for production use.
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef TASKDSP
#define TASKDSP

#include <atomic>
#include <bitset>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include <base/common.h>
#include <base/taskmanager/dispatcher/TaskManagerTypes.h>
#include <base/taskmanager/thread/platform/common/PlatformThread.h>

namespace base { namespace threading {
	class ConditionVariable;
}
} // namespace base::threading

namespace base { namespace taskmanager {
	class TaskManager {
	public:
		//Initializes the TaskManager. threadPoolSize > 0 will create a threadpool with threadPoolSize number of threads. If threadPoolSize is greater than the number of
		//hardware threads available, it will be clamped to that number. If threadPoolSize == 0 no thread pool will be created and any tasks posted through
		//PostThreadPoolTask will instead run on the main thread. You can also set threadPoolSize to -1 to to force it to use all available hardware threads.
		void InitializeThreadPool(int threadPoolSize);
		//Takes over the current thread and creates a new task queue and runner for it with the specified name.
		//If initTask is not null, it will the first task executed on the new task runner.
		void InitializeOnCurrentThread(std::string queueName, Task* initTask);
		//Creates a new Runner on a new dedicated DispatcherThread with the name specified by 'name'.
		//After creation the task pointed to by 'initTask' will be executed on the new thread.
		void CreateRunnerOnNewThread(std::string name, Task* initTask);
		//Posts a task for execution and doesn't care what thread it executes on.
		//If there's no threadpool for this task to execute on it will run on the main thread.
		void PostThreadPoolTask(Task* task);
		//Posts a task for execution to 'queueName'.
		void PostNonThreadPoolTask(Task* task, std::string queueName);

	private:
		SINGLETON(TaskManager)
		//Task runners are responsible for running a task, either on a thread pool thread, or on a dedicated thread.
		//Task runners that are created on a thread pool owned thread will pull tasks from the pool's task queue.
		//Task runners that are created on a dedicated thread will pull tasks from their own queue.
		class Runner {
		public:
			Runner(bool forPoolThread);
			void Signal();
			void StartTaskRunner();
			void PostTask(Task* task);
			;
			//TODO: Does this need to be guarded? Hmm..
			void StopRunning() { keepRunning = false; }
			void SetExecutionStatus() { inUse.store(true); }
			bool GetExecutionStatus() { return inUse.load(); }
			void InitRunnerForThreadPool(threading::ConditionVariable* taskSignal);

		private:
			void ExecuteRegularTask();
			void ExecuteThreadPoolTask();
			std::atomic<bool> inUse;
			bool keepRunning = true;
			std::unique_ptr<Task> pendingTask;
			bool threadPoolTaskRunner = false;
			base::threading::ConditionVariable* newTaskSignal;
			std::unique_ptr<std::priority_queue<Task, std::vector<Task>, Task::PriorityComparer>> queue;
		};
		//A Thread represents a single thread of execution.
		class Thread : public PlatformThread::Delegate {
		public:
			Thread(TaskManager::Runner* runner, std::string name);
			void PostTask(Task* task);
			void ThreadMain() override;

		private:
			void StartDispatchLoop();
			TaskManager::Runner* taskRunner;
		};
		//A thread pool creates and manages a number of threads for executing tasks from single task queue that it owns.
		class ThreadPool {
		public:
			ThreadPool(int threadCount);
			void PostTask(Task* task);
			void ExecuteTasks();

		private:
			int GetUnusedThread();
			void QueuePop() {
				std::scoped_lock lock(queueLock);
				LOG_MSG("queue pop")
				queue->pop();
			}
			std::bitset<255> inUseThreads;
			std::mutex queueLock;
			base::threading::ConditionVariable* newTaskSignal;
			std::unordered_map<int, std::unique_ptr<TaskManager::Runner>> runners;
			std::unordered_map<int, std::unique_ptr<TaskManager::Thread>> threads;
			std::unique_ptr<std::priority_queue<Task, std::vector<Task>, Task::PriorityComparer>> queue;
		};
		TaskManager::ThreadPool* threadPool;
		std::unordered_map<std::string, std::unique_ptr<TaskManager::Runner>> runners;
		std::unordered_map<std::string, std::unique_ptr<TaskManager::Thread>> threads;
	};
}
} // namespace base::taskmanager

#endif