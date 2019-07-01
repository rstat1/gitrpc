/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GITSERVER
#define GITSERVER

#include <base/app/app_base.h>

namespace nexus {
	using namespace base::app;
	class App {
		public:
			App(std::function<void()> appMain) {
				app = new App::Impl(appMain);
			}
			APP_BASE(Impl)
				public:
					void TaskRunnerInitComplete() override
					{
						LOG_MSG("In TaskRunnerInitComplete");
						this->appFunc();
					}
			END_APP_BASE()
		private:
			App::Impl* app;
	};
}

#endif