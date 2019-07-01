/*
* Copyright (c) 2014 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <errno.h>
#include <base/common.h>
#include <sys/syscall.h>
#include <android/looper.h>
#include <base/threading/ConditionVariable.h>
#include <platform/android/AndroidAppState.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

using namespace platform::android;

namespace base { namespace threading 
{
	bool DispatcherMessagePump::msgPumpWinInit;
	int LooperCallback(int fd, int events, void* data)
	{
		DispatcherTask* task;		
		AndroidAppState* ahs = (AndroidAppState*) data;

		Log("XPAppFX-Native", "Running task on thread id: %u", syscall(__NR_gettid));

		pthread_mutex_lock(&ahs->blockFDWrites);
		if (read(ahs->fds[0], &task, sizeof(task)) == sizeof(task))
		{
			if (task->HasCallback()) { task->InvokeWithCallback(); }
			else { task->Invoke(); }
		}
		pthread_mutex_unlock(&ahs->blockFDWrites);

		return 1;
	}
	void MessagePumpAndroid::MakeMessagePump()
	{
		Thread* AppHostThread = Dispatcher::Get()->GetThread(this->wndIdExtenstion);
		AndroidAppState* ahs = (AndroidAppState*) AppHostThread->extra;
		ALooper* looper = ALooper_prepare(0);
		
		if (ahs == nullptr) { writeToLog("Why is the null?"); }
		else
		{
			ahs->threadLooper = looper;
			pthread_mutex_init(&ahs->blockFDWrites, NULL);

			pipe(ahs->fds);
			ALooper_addFd(ahs->threadLooper, ahs->fds[0], 1, ALOOPER_EVENT_INPUT, LooperCallback, ahs);
		}		

		this->StartMessageLoop();
	}
	void MessagePumpAndroid::PreInitMessagePump()
	{
	}
	void MessagePumpAndroid::StartMessageLoop()
	{
		Log("Dispatcher", "Current thread id = %u, thread name = %s", syscall(__NR_gettid), this->wndIdExtenstion);

		int ident;
		int events;
		DispatcherTask* eventData;

		this->startComplete = true;
		this->postBlocker->Signal();

		ident = ALooper_pollAll(-1, NULL, &events, (void**) &eventData);
		writeToLog("Oh look an event");

	}
	void MessagePumpAndroid::PostMessageToThread(const char* thread, DispatcherTask *task)
	{	
		if (this->startComplete == false) { this->postBlocker->Wait(); }

		Thread* AppHostThread = Dispatcher::Get()->GetThread(thread);
		if (AppHostThread == nullptr) { Log("XPAppFX-Native", "wat? Thread: %s doesn't real ", thread); }
		else
		{
			AndroidAppState* ahs = (AndroidAppState*) AppHostThread->extra;

			pthread_mutex_lock(&ahs->blockFDWrites);
			write(ahs->fds[1], &task, sizeof(task));
			pthread_mutex_unlock(&ahs->blockFDWrites);
		}
	}
}}