/*
* Copyright (c) 2013 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/common.h>

#ifndef THRDTYPES
#define THRDTYPES

#if defined(OS_WIN)
#include <Windows.h>
typedef LPVOID LPVOID;
typedef HANDLE PlatformMutex;
typedef HANDLE platformThread;
typedef LPTHREAD_START_ROUTINE ThreadProc;
typedef unsigned long SlotType;
typedef DWORD ThreadID;
#define ThreadFunction LPTHREAD_START_ROUTINE ThreadStartFunction
#endif

#if defined(OS_ANDROID) || defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <pthread.h>
typedef void* LPVOID;
typedef pthread_mutex_t PlatformMutex;
typedef pthread_t platformThread;
typedef pthread_t ThreadID;
typedef void* ThreadProc;
typedef pthread_key_t SlotType;
#define ThreadFunction void *(*ThreadStartFunction)(void *)
#define EBUSY  16
#define EINVAL 22
#endif

#endif