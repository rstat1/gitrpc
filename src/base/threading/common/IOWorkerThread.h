/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef WORKTHRD
#define WORKTHRD

#include <base/base_exports.h>
#include <base/threading/common/IOTasks.h>
#include <base/threading/common/PlatformThread.h>

using namespace base::threading;

namespace base { namespace threading { namespace IO
{
	class BASEAPI IOWorkerThread : public PlatformThread::Delegate
	{
		public:
			IOWorkerThread() {}
			IOWorkerThread(IOTaskBase* task);
			void Start(const char* name) OVERRIDE;
			void ThreadMain() OVERRIDE;
	};
}}}

#endif