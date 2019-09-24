/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef BASEAPP
#define BASEAPP

#include <base/common.h>
#include <base/threading/common/TaskRunner.h>
#include <base/threading/dispatcher/Dispatcher.h>

#define APP_BASE(name)            \
	class name : public AppBase { \
	public:                       \
		name(std::function<void()> mainFunc) : AppBase(mainFunc) { this->InitAppBase(); }
#define END_APP_BASE() \
	}                  \
	;
namespace base { namespace app {
	using namespace base::threading;
	class AppBase {
	public:
		AppBase(std::function<void()> AppMainFunc) {
			LOG_MSG("In AppBase ctor");
			this->appFunc = AppMainFunc;
		}
		void InitAppBase();
		void StartTaskRunner() { this->taskRunner->Start(); }
		virtual void TaskRunnerInitComplete() = 0;

	protected:
		std::function<void()> appFunc;

	private:
		TaskRunner* taskRunner;
	};
}} // namespace base::app

#endif