/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MPUMPWIN
#define MPUMPWIN

#include <windows.h>
//#include <base/threading/dispatcher/DispatcherTypes.h>
//#include <base/threading/platform/DispatcherTypesWin.h>
#include <base/threading/dispatcher/DispatcherMessagePump.h>

namespace base { namespace threading
{
	class BASEAPI MessagePumpWin : public DispatcherMessagePump
	{
		public:
			MessagePumpWin(const char* winIDExt) : DispatcherMessagePump(winIDExt) {}
			void MakeMessagePump(bool isTaskRunner) override;
			void MakeMessagePump(DispatcherTask* InitTask, bool isTaskRunner) override;
			void StartMessageLoop(bool isTaskRunner) override;
			void PostMessageToThread(const char* thread, DispatcherTask *task, bool isTaskRunner) override;
			void RegisterMessageHandler(MessageReceiver* recv) { this->handlers[recv->filter] = recv; }
			static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		private:
  			ATOM wndClassATOM;
			static std::map<HWND, MessageReceiver*> handlers;
	};
}}

#endif