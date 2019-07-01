/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef DISPTYPES
#define DISPTYPES

#include <any>
#include <base/common.h>
#include <base/threading/dispatcher/Dispatcher.h>

#define THREAD_CHECK(name) assert(Dispatcher::Get()->IsCorrectThread(name) == true);
#define POST_TASK(task, threadName) Dispatcher::Get()->PostTask(threadName, task)
#define NEW_TASK0(Name, TaskType, TaskTypeRef, Function) DispatcherTask* Name = DispatcherTask::CreateTask<TaskType, &TaskType::Function>(TaskTypeRef)
#define NEW_TASK1(Name, TaskType, TaskTypeRef, Function, Args) DispatcherTask* Name = DispatcherTask::CreateTask<TaskType, &TaskType::Function>(TaskTypeRef, Args)

namespace base { namespace threading {
	struct TaskResult {
	public:
		std::any result;
		TaskResult() {}
		TaskResult(std::any value) { result = value; }
	};
	class DispatcherTask {
		typedef void* PointerToObject;
		typedef TaskResult* (*TaskFunction)(PointerToObject, void*);
		typedef TaskResult* (*TaskFunctionNoArgs)(PointerToObject);
		typedef void (*TaskCBFunction)(PointerToObject, TaskResult*);
		typedef void (*TaskFuncNoReturn)(PointerToObject, void*);
		typedef void (*TaskNoAnything)(PointerToObject);

	public:
		TaskCBFunction cbFunction;
		TaskFunction taskFunction;
		TaskNoAnything taskNothing;
		TaskFunctionNoArgs taskFuncNoArgs;
		TaskFuncNoReturn taskFuncNoReturn;

		template <TaskResult* (*Function)(void*)>
		static DispatcherTask* CreateTask(void* args) {
			DispatcherTask* dt = new DispatcherTask();
			dt->Bind<Function>(args);
			return dt;
		}
		template <class C, void (C::*Function)(void*)>
		static DispatcherTask* CreateTask(C* reference, void* args) {
			DispatcherTask* dt = new DispatcherTask();
			dt->Bind<C, Function>(reference, args);
			return dt;
		}
		template <class C, TaskResult* (C::*Function)(void*)>
		static DispatcherTask* CreateTask(C* reference, void* args) {
			DispatcherTask* dt = new DispatcherTask();
			dt->Bind<C, Function>(reference, args);
			return dt;
		}
		template <class C, void (C::*Function)()>
		static DispatcherTask* CreateTask(C* reference) {
			DispatcherTask* dt = new DispatcherTask();
			dt->Bind<C, Function>(reference);
			return dt;
		}
		template <class C, TaskResult* (C::*Function)()>
		static DispatcherTask* CreateTask(C* reference) {
			DispatcherTask* task = new DispatcherTask();
			task->Bind<C, Function>(reference);
			return task;
		}
		template <class TaskType, class Callback, TaskResult* (TaskType::*TaskFunc)(void*), void (Callback::*CBFunc)(TaskResult*)>
		static DispatcherTask* CreateTaskCB(TaskType* taskRef, Callback* cbRef, void* args, const char* callbackThreadName) {
			DispatcherTask* dt = new DispatcherTask();
			dt->Bind<TaskType, Callback, TaskFunc, CBFunc>(taskRef, cbRef, args, callbackThreadName);
			return dt;
		}
		template <class TaskType, class Callback, TaskResult* (TaskType::*TaskFunc)(), void (Callback::*CBFunc)(TaskResult*)>
		static DispatcherTask* CreateTaskCB(TaskType* taskRef, Callback* cbRef, const char* callbackThreadName) {
			DispatcherTask* dt = new DispatcherTask();
			dt->Bind<TaskType, Callback, TaskFunc, CBFunc>(taskRef, cbRef, callbackThreadName);
			return dt;
		}
		template <class C, TaskResult* (C::*Function)()>
		void Bind(C* reference) {
			taskFuncInstance = reference;
			functionArguments = nullptr;
			hasCB = false;
			hasArgs = false;
			isCBTask = false;
			taskFuncNoArgs = &ClassMethod<C, Function>;
		}
		template <TaskResult* (*Function)(void*)>
		void Bind(void* args) {
			taskFuncInstance = nullptr;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = true;
			taskFunction = &Method<Function>;
		}
		template <class C, TaskResult* (C::*Function)(void*)>
		void Bind(C* reference, void* args) {
			taskFuncInstance = reference;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = true;
			taskFunction = &ClassMethod<C, Function>;
		}
		template <class Task, class Callback, TaskResult* (Task::*Function)(void*), void (Callback::*CBFunc)(TaskResult*)>
		void Bind(Task* TaskReference, Callback* CallbackReference, void* args, const char* callbackThreadName) {
			taskFuncInstance = TaskReference;
			cbFuncInstance = CallbackReference;
			functionArguments = args;
			cbThreadName = callbackThreadName;
			hasCB = true;
			hasArgs = true;
			taskFunction = &ClassMethod<Task, Function>;
			cbFunction = &ClassMethod<Callback, CBFunc>;
		}
		template <class Task, class Callback, TaskResult* (Task::*Function)(), void (Callback::*CBFunc)(TaskResult*)>
		void Bind(Task* TaskReference, Callback* CallbackReference, const char* callbackThreadName) {
			taskFuncInstance = TaskReference;
			cbFuncInstance = CallbackReference;
			functionArguments = nullptr;
			cbThreadName = callbackThreadName;
			hasCB = true;
			taskFuncNoArgs = &ClassMethod<Task, Function>;
			cbFunction = &ClassMethod<Callback, CBFunc>;
		}
		template <class C, void (C::*Function)(void*)>
		void Bind(C* reference, void* args) {
			taskFuncInstance = reference;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = false;
			taskFuncNoReturn = &ClassMethod<C, Function>;
		}
		template <class C, void (C::*Function)()>
		void Bind(C* reference) {
			taskFuncInstance = reference;
			functionArguments = nullptr;
			hasCB = false;
			hasArgs = false;
			isCBTask = false;
			hasReturn = false;
			taskNothing = &ClassMethod<C, Function>;
		}
		template <TaskResult* (*Function)()>
		static void Method(PointerToObject) {
			(Function)();
		}
		template <TaskResult* (*Function)(void*)>
		static void Method(PointerToObject, void* args) {
			(Function)(args);
		}
		template <class C, TaskResult* (C::*Function)()>
		static TaskResult* ClassMethod(PointerToObject ptr) {
			return (static_cast<C*>(ptr)->*Function)();
		}
		template <class C, TaskResult* (C::*Function)(void*)>
		static TaskResult* ClassMethod(PointerToObject ptr, void* args) {
			return (static_cast<C*>(ptr)->*Function)(args);
		}
		template <class C, void (C::*Function)(TaskResult*)>
		static void ClassMethod(PointerToObject ptr, TaskResult* args) {
			return (static_cast<C*>(ptr)->*Function)(args);
		}
		template <class C, void (C::*Function)(void*)>
		static void ClassMethod(PointerToObject ptr, void* args) {
			return (static_cast<C*>(ptr)->*Function)(args);
		}
		template <class C, void (C::*Function)()>
		static void ClassMethod(PointerToObject ptr) {
			return (static_cast<C*>(ptr)->*Function)();
		}
		void Invoke(bool NoArguments) {
			if (functionArguments != nullptr) {
				if (!isCBTask) {
					if (hasReturn) {
						taskFunction(taskFuncInstance, functionArguments);
					} else {
						taskFuncNoReturn(taskFuncInstance, functionArguments);
					}
				} else {
					cbFunction(cbFuncInstance, (TaskResult*)functionArguments);
				}
			} else {
				if (hasReturn) {
					taskFuncNoArgs(taskFuncInstance);
				} else {
					taskNothing(taskFuncInstance);
				}
			}
		}
		void InvokeWithCallback(bool NoArguments) {
			TaskResult* tr;

			if (!NoArguments) {
				tr = taskFunction(taskFuncInstance, functionArguments);
			} else {
				tr = taskFuncNoArgs(taskFuncInstance);
			}

			Dispatcher::Get()->PostTaskToThread(DispatcherTask::CreateCallbackTask(tr, cbFuncInstance, cbFunction), cbThreadName);
		}
		//Shouldn't be run in a message loop as it just return to the message loop function that called it.
		TaskResult* InvokeAndReturn(bool NoArguments) {
			if (!NoArguments) {
				return taskFunction(taskFuncInstance, functionArguments);
			} else {
				return taskFuncNoArgs(taskFuncInstance);
			}
		}
		bool HasCallback() { return hasCB; }
		bool HasArguments() { return hasArgs; }
		const char* GetCallbackThreadName() { return cbThreadName; }

	private:
		DispatcherTask* CreateCallbackTask(TaskResult* tr, PointerToObject reference, TaskCBFunction callback) {
			DispatcherTask* cbTask = new DispatcherTask();
			cbTask->CallbackBind(reference, callback, tr);
			return cbTask;
		}
		void CallbackBind(PointerToObject reference, TaskCBFunction cb, TaskResult* args) {
			cbFuncInstance = reference;
			functionArguments = args;
			cbFunction = cb;
			isCBTask = true;
			hasReturn = false;
			hasCB = false;
		}
		DispatcherTask() {}
		PointerToObject taskFuncInstance, cbFuncInstance;

		void* functionArguments;
		bool hasCB = false, isCBTask = false, hasArgs = false, hasReturn = true;
		const char* cbThreadName;
	};
}} // namespace base::threading

#endif