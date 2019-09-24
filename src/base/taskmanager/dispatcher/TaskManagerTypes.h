/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef DISPTYPES
#define DISPTYPES

#include <any>
#include <base/Utils.h>
#include <base/common.h>
#include <functional>
// #include <base/threading/dispatcher/Dispatcher.h>

// #define THREAD_CHECK(name) assert(Dispatcher::Get()->IsCorrectThread(name) == true);
// #define POST_TASK(task, threadName) Dispatcher::Get()->PostTask(threadName, task)
#define NEW_TASK0(Name, TaskType, TaskTypeRef, Function, TPriority) base::taskmanager::Task* Name = base::taskmanager::Task::Create<TaskType, &TaskType::Function>(TaskTypeRef, base::taskmanager::Task::Priority::TPriority)
#define NEW_TASK1(Name, TaskType, TaskTypeRef, Function, Args) base::taskmanager::Task* Name = base::taskmanager::Task::Create<TaskType, &TaskType::Function>(TaskTypeRef, Args)

namespace base { namespace taskmanager {
	struct TaskResult {
	public:
		std::any result;
		TaskResult() {}
		TaskResult(std::any value) { result = value; }
	};
	typedef void* PointerToObject;
	typedef TaskResult* (*TaskFunction)(PointerToObject, void*);
	typedef TaskResult* (*TaskFunctionNoArgs)(PointerToObject);
	typedef void (*TaskCBFunction)(PointerToObject, TaskResult*);
	typedef void (*TaskFuncNoReturn)(PointerToObject, void*);
	typedef void (*TaskNoAnything)(PointerToObject);
	//Represents the details of a single task. Initialize it by using one of the many Create(*) functions, or with the NEW_TASK* macros.
	class Task {
	public:
		enum class Priority {
			High = 0,
			Normal,
			Low,
		};
		struct PriorityComparer {
			bool operator()(Task& x, Task& y) {
				return std::greater<int>()((int)x.GetTaskPriority(), (int)y.GetTaskPriority());
			}
		};
		TaskCBFunction cbFunction;
		TaskFunction taskFunction;
		TaskNoAnything taskNothing;
		TaskFunctionNoArgs taskFuncNoArgs;
		TaskFuncNoReturn taskFuncNoReturn;

		template <TaskResult* (*Function)(void*)>
		static Task* Create(void* args, Task::Priority priority) {
			Task* dt = new Task();
			dt->Bind<Function>(args, priority);
			return dt;
		}
		template <class ClassType, void (ClassType::*Function)(void*)>
		static Task* Create(ClassType* reference, void* args, Task::Priority priority) {
			Task* dt = new Task();
			dt->Bind<ClassType, Function>(reference, args, priority);
			return dt;
		}
		template <class ClassType, TaskResult* (ClassType::*Function)(void*)>
		static Task* Create(ClassType* reference, void* args, Task::Priority priority) {
			Task* dt = new Task();
			dt->Bind<ClassType, Function>(reference, args, priority);
			return dt;
		}
		template <class ClassType, void (ClassType::*Function)()>
		static Task* Create(ClassType* reference, Task::Priority priority) {
			Task* dt = new Task();
			dt->Bind<ClassType, Function>(reference, priority);
			return dt;
		}
		template <class ClassType, TaskResult* (ClassType::*Function)()>
		static Task* Create(ClassType* reference, Task::Priority priority) {
			Task* task = new Task();
			task->Bind<ClassType, Function>(reference, priority);
			return task;
		}
		void Invoke() {
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
			isComplete = true;
		}
		void InvokeWithCallback(bool NoArguments) {
			TaskResult* tr;

			if (!NoArguments) {
				tr = taskFunction(taskFuncInstance, functionArguments);
			} else {
				tr = taskFuncNoArgs(taskFuncInstance);
			}

			// Dispatcher::Get()->PostTaskToThread(Task::CreateCallbackTask(tr, cbFuncInstance, cbFunction), cbThreadName);
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
		Priority GetTaskPriority() const { return taskPriority; }
		bool IsCompleted() { return isComplete; }
		std::string GetTaskID() { return taskID; }

	private:
		Priority taskPriority;
		std::string taskID;
		bool isComplete = false;
		template <class ClassType, TaskResult* (ClassType::*Function)()>
		void Bind(ClassType* reference, Task::Priority priority) {
			taskFuncInstance = reference;
			functionArguments = nullptr;
			hasCB = false;
			hasArgs = false;
			isCBTask = false;
			taskPriority = priority;
			taskFuncNoArgs = &ClassMethod<ClassType, Function>;
		}
		template <TaskResult* (*Function)(void*)>
		void Bind(void* args, Task::Priority priority) {
			taskFuncInstance = nullptr;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = true;
			taskPriority = priority;
			taskFunction = &Method<Function>;
		}
		template <class ClassType, TaskResult* (ClassType::*Function)(void*)>
		void Bind(ClassType* reference, void* args, Task::Priority priority) {
			taskFuncInstance = reference;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = true;
			taskPriority = priority;
			taskFunction = &ClassMethod<ClassType, Function>;
		}
		template <class ClassType, void (ClassType::*Function)(void*)>
		void Bind(ClassType* reference, void* args, Task::Priority priority) {
			taskFuncInstance = reference;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = false;
			taskFuncNoReturn = &ClassMethod<ClassType, Function>;
		}
		template <class ClassType, void (ClassType::*Function)()>
		void Bind(ClassType* reference, Task::Priority priority) {
			taskFuncInstance = reference;
			functionArguments = nullptr;
			hasCB = false;
			hasArgs = false;
			isCBTask = false;
			hasReturn = false;
			taskPriority = priority;
			taskNothing = &ClassMethod<ClassType, Function>;
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

		Task* CreateCallbackTask(TaskResult* tr, PointerToObject reference, TaskCBFunction callback) {
			Task* cbTask = new Task();
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
		Task() {
			taskID = base::utils::GenerateRandomString(16);
		}
		PointerToObject taskFuncInstance, cbFuncInstance;

		void* functionArguments;
		bool hasCB = false, isCBTask = false, hasArgs = false, hasReturn = true;
		const char* cbThreadName;
	};
}} // namespace base::taskmanager

#endif