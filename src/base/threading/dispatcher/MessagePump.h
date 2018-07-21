/*
* Copyright (c) 2016 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MSGPMP
#define MSGPMP

#include <base/common.h>
#include <base/threading/dispatcher/DispatcherMessagePump.h>

#if defined(OS_WIN)
#include <base/platform/win/dispatcher/MessagePumpWin.h>
#define MESSAGE_PUMP_INIT(name) dmp = new MessagePumpWin(name)
#define MESSAGE_PUMP_CREATE(name) DispatcherMessagePump* dmp = new MessagePumpWin(name)
#elif defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <base/platform/linux/dispatcher/MessagePumpEPoll.h>
#define MESSAGE_PUMP_INIT(name) dmp = new MessagePumpEPoll(name)
#define MESSAGE_PUMP_CREATE(name) DispatcherMessagePump* dmp = new MessagePumpEPoll(name)
#define NEW_MP(name, initTask) MESSAGE_PUMP_INIT(name); \
							   Dispatcher::Get()->AddMessagePump(name, dmp, base::utils::GetThreadID()); \
							   dmp->MakeMessagePump(initTask, true);

#elif defined(OS_ANDROID)
#include <base/platform/android/dispatcher/MessagePumpAndroid.h>
#define MESSAGE_PUMP_INIT(name) dmp = new MessagePumpAndroid(name)
#define MESSAGE_PUMP_CREATE(name) DispatcherMessagePump* dmp = new MessagePumpAndroid(name)
#endif

#endif