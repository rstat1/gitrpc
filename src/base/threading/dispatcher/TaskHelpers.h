/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef TASKHELP
#define TASKHELP

#include <base/threading/dispatcher/DispatcherTypes.h>

namespace base { namespace threading {
	template <class TaskType, void (TaskType::*Function)()>
	Task* CreateTask(TaskType* ref) {
		return Task::Create<TaskType, Function>(ref, TaskPriority::LOW);
	}
	template <class TaskType, void (TaskType::*Function)()>
	Task* CreateTask(TaskType* ref, void* args) {
		return Task::Create<TaskType, Function>(ref, args, TaskPriority::LOW);
	}
	template <class TaskType, void (TaskType::*Function)()>
	Task* CreateRankedTask(TaskType* ref, TaskPriority priority) {
		return Task::Create<TaskType, Function>(ref, priority);
	}
	template <typename TaskType, void (TaskType::*Function)(void*)>
	Task* CreateRankedTask(TaskType* ref, void* args, TaskPriority priority) {
		return Task::Create<TaskType, Function>(ref, args, priority);
	}

}} // namespace base::threading

#endif