/*
* Copyright (c) 2012 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/threading/common/ConditionVariable.h>

namespace base { namespace threading
{
	ConditionVariable::ConditionVariable()
	{
		pthread_mutex_init(&m, NULL);
		pthread_cond_init(&condition, NULL);
	}
	ConditionVariable::ConditionVariable(PlatformMutex mutex) {}
    void ConditionVariable::Wait()
    {
       	pthread_cond_wait(&this->condition, &this->m);
    }
   	void ConditionVariable::Signal()
   	{
       	pthread_cond_signal(&this->condition);
   	}
}}
