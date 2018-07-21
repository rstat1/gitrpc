/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/Utils.h>
#include <base/threading/common/IOWorkerThread.h>

namespace base { namespace threading { namespace IO
{
	IOWorkerThread::IOWorkerThread(IOTaskBase* task)
	{
		this->Id = base::utils::GetThreadID();
		#if defined(OS_LINUX) || defined(OS_STEAMLINK)
		this->sts = new platform::SharedThreadState();
		#endif
		this->extra = task;
	}
	void IOWorkerThread::Start(const char* name)
	{
		if (!PlatformThread::Create(this, name)) { writeToLog("Thread creation failed."); }
	}
	void IOWorkerThread::ThreadMain()
	{
		IOTaskBase* task = (IOTaskBase*)this->extra;
		task->ExecuteTask();
	}
}}}