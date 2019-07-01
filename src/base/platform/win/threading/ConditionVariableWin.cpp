/*
* Copyright (c) 2012 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/threading/common/ConditionVariable.h>

namespace base { namespace threading {
	ConditionVariable::ConditionVariable() {
		InitializeConditionVariable(&c);
	}
	ConditionVariable::ConditionVariable(PlatformMutex mutex) {}
	void ConditionVariable::Wait() {
		EnterCriticalSection(&cs);
		SleepConditionVariableCS(&c, &cs, INFINITE);
	}
	void ConditionVariable::Signal() {
		LeaveCriticalSection(&cs);
		WakeConditionVariable(&c);
	}
}} // namespace base::threading