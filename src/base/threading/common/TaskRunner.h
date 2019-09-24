/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef SMPTHRD
#define SMPTHRD

#include <base/threading/dispatcher/MessagePump.h>

namespace base { namespace threading {
	class BASEAPI TaskRunner {
	public:
		TaskRunner();
		void Start();
		void RunTask(Task *task);
		void Init(const char *taskRunnerName, Task *TaskRunnerInitMethod);
		void *extra;
		ThreadID Id;
		DispatcherMessagePump *dmp;

	private:
		const char *runnerName;
		Task *initTask;
	};
}} // namespace base::threading

#endif