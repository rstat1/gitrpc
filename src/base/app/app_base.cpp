/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/app/app_base.h>
#include <base/threading/common/TaskRunner.h>
#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

namespace base { namespace app {
	void AppBase::InitAppBase() {
		LOG_MSG("Init app")
		NEW_TASK0(initTask, AppBase, this, TaskRunnerInitComplete);
		this->taskRunner = new TaskRunner();
		this->taskRunner->Init("Main", initTask);
		this->taskRunner->Start();
	}
}} // namespace base::app