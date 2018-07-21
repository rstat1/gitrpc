/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef IOTSKS
#define IOTSKS

#include <future>
#include <functional>
#include <base/threading/dispatcher/DispatcherTypes.h>

using namespace base::threading;

namespace base { namespace threading { namespace IO
{
    class IOTaskBase
    {
        public:
            virtual void ExecuteTask() = 0;
		protected:
			std::function<void(TaskResult*)> resultCallback;
			DispatcherTask* task;
    };

}}}

#endif