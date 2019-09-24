/*
* Copyright (c) 2014-2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef DISPTYPES
#define DISPTYPES

#include <any>

#include <base/common.h>

#define THREAD_CHECK(name) assert(Dispatcher::Get()->IsCorrectThread(name) == true);
#define POST_TASK(task, threadName) Dispatcher::Get()->PostTask(threadName, task)
#define RANKED_TASK0(Name, Type, Ref, Func, TPriority) Task* Name = \
														   Task::Create<Type, &Type::Func>(Ref, TPriority)

#define RANKED_TASK1(Name, Type, Ref, Func, TPriority, Args) Task* Name = \
																 Task::Create<Type, &Type::Func>(Ref, Args, TPriority)

#define NEW_TASK0(Name, TaskType, TaskTypeRef, Function) RANKED_TASK0(Name, TaskType, TaskTypeRef, Function, TaskPriority::LOW)
#define NEW_TASK1(Name, TaskType, TaskTypeRef, Function, Args) RANKED_TASK1(Name, TaskType, TaskTypeRef, Function, TaskPriority::LOW, Args)

// #define NAMEOF(T) #T

#define DEBUG_NEWTASK1(Name, TaskType, TaskTypeRef, Function, Args, FuncNameAsStr) \
	LOG_MSG(FuncNameAsStr)                                                         \
	RANKED_TASK1(Name, TaskType, TaskTypeRef, Function, TaskPriority::LOW, Args)

namespace base { namespace threading {
	enum class TaskPriority {
		HIGH = 0,
		NORMAL,
		LOW,
	};
	struct TaskResult {
	public:
		std::any result;
		TaskResult() {}
		TaskResult(std::any value) { result = value; }
	};
	class Task {
	public:
		typedef void* PointerToObject;
		typedef TaskResult* (*TaskFunction)(PointerToObject, void*);
		typedef TaskResult* (*TaskFunctionNoArgs)(PointerToObject);
		typedef void (*TaskCBFunction)(PointerToObject, TaskResult*);
		typedef void (*TaskFuncNoReturn)(PointerToObject, void*);
		typedef void (*TaskNoAnything)(PointerToObject);
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
		static Task* Create(void* args, TaskPriority priority) {
			Task* dt = new Task();
			dt->Bind<Function>(args, priority);
			return dt;
		}
		template <class ClassType, void (ClassType::*Function)(void*)>
		static Task* Create(ClassType* reference, void* args, TaskPriority priority) {
			Task* dt = new Task();
			dt->Bind<ClassType, Function>(reference, args, priority);
			return dt;
		}
		template <class ClassType, TaskResult* (ClassType::*Function)(void*)>
		static Task* Create(ClassType* reference, void* args, TaskPriority priority) {
			Task* dt = new Task();
			dt->Bind<ClassType, Function>(reference, args, priority);
			return dt;
		}
		template <class ClassType, void (ClassType::*Function)()>
		static Task* Create(ClassType* reference, TaskPriority priority) {
			Task* dt = new Task();
			dt->Bind<ClassType, Function>(reference, priority);
			return dt;
		}
		template <class ClassType, TaskResult* (ClassType::*Function)()>
		static Task* Create(ClassType* reference, TaskPriority priority) {
			Task* task = new Task();
			task->Bind<ClassType, Function>(reference, priority);
			return task;
		}
		template <class ClassType, TaskResult* (ClassType::*Function)()>
		void Bind(ClassType* reference, TaskPriority priority) {
			taskFuncInstance = reference;
			functionArguments = nullptr;
			hasCB = false;
			hasArgs = false;
			isCBTask = false;
			taskPriority = priority;

			taskFuncNoArgs = &ClassMethod<ClassType, Function>;
		}
		template <TaskResult* (*Function)(void*)>
		void Bind(void* args, TaskPriority priority) {
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
		void Bind(ClassType* reference, void* args, TaskPriority priority) {
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
		void Bind(ClassType* reference, void* args, TaskPriority priority) {
			taskFuncInstance = reference;
			functionArguments = args;
			hasCB = false;
			hasArgs = true;
			isCBTask = false;
			hasReturn = false;
			taskFuncNoReturn = &ClassMethod<ClassType, Function>;
		}
		template <class ClassType, void (ClassType::*Function)()>
		void Bind(ClassType* reference, TaskPriority priority) {
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
			isComplete = true;
		}
		bool HasCallback() { return hasCB; }
		bool HasArguments() { return hasArgs; }
		bool IsComplete() { return isComplete; }
		const char* GetCallbackThreadName() { return cbThreadName; }
		TaskPriority GetTaskPriority() const { return taskPriority; }

	private:
		TaskPriority taskPriority;
		void CallbackBind(PointerToObject reference, TaskCBFunction cb, TaskResult* args) {
			cbFuncInstance = reference;
			functionArguments = args;
			cbFunction = cb;
			isCBTask = true;
			hasReturn = false;
			hasCB = false;
		}
		Task() {}
		PointerToObject taskFuncInstance, cbFuncInstance;

		void* functionArguments;
		bool hasCB = false, isCBTask = false, hasArgs = false, hasReturn = true, isComplete = false;
		const char* cbThreadName = "";
	};
}
} // namespace base::threading

#endif