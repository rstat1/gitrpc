/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/Utils.h>
//#include <base/threading/common/TaskRunner.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

#if defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/platform/linux/dispatcher/SharedThreadState.h>
using namespace platform;
#endif

namespace base { namespace threading {
	TaskRunner::TaskRunner() {}
	void TaskRunner::Init(const char *taskRunnerName, DispatcherTask *TaskRunnerInitMethod) {
#if !defined(OS_WIN)
		this->Id = base::utils::GetPthreadID();
#else
		this->Id = base::utils::GetThreadID();
#endif
		this->runnerName = taskRunnerName;
		this->initTask = TaskRunnerInitMethod;

#if defined(OS_LINUX) || defined(OS_STEAMLINK)
		SharedThreadState *sts = new SharedThreadState();
		sts->blockUntilAHInit = new ConditionVariable();
		this->extra = sts;
#endif
		MESSAGE_PUMP_INIT(taskRunnerName);

		Dispatcher::Get()->AddTaskRunner(taskRunnerName, this);
	}
	void TaskRunner::Start() {
		this->dmp->MakeMessagePump(this->initTask, true);
	}
	void TaskRunner::RunTask(DispatcherTask *task) {
		dmp->PostMessageToThread(this->runnerName, task, true);
	}
}} // namespace base::threading
