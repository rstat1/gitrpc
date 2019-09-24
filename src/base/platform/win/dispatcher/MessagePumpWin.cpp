/*
* Copyright (c) 2014 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/platform/win/dispatcher/MessagePumpWin.h>

#include <base/common.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

using namespace std::placeholders;

namespace base { namespace threading {
	static const int newTaskMessage = WM_USER + 1;
	std::map<HWND, MessageReceiver *> MessagePumpWin::handlers;
	bool DispatcherMessagePump::msgPumpWinInit;

	LRESULT CALLBACK ThreadMessageWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		Task *task;

		if (message == newTaskMessage) {
			task = (Task *)lParam;
			//TODO: Task with no args?
			if (task->HasCallback()) {
				task->InvokeWithCallback(!task->HasArguments());
			} else {
				task->Invoke(!task->HasArguments());
			}
		} else {
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
	LRESULT CALLBACK MessagePumpWin::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		MessageReceiver *recv;

		auto it = handlers.find(hwnd);
		if (it == handlers.end()) {
			switch (message) {
				case WM_CREATE:
					//Log("Dispatcher", "Using generic WM_CREATE handler for HWND: %d because message filter isn't registered yet", hwnd);
					return true;
					break;
				case WM_PAINT:
					//Log("Dispatcher", "Using generic WM_PAINT handler for HWND: %d because message filter isn't registered yet", hwnd);
					return true;
					break;
				case WM_NCCREATE:
					return true;
					break;
				case WM_CLOSE:
					LOG_MSG("No available handler for message");
					break;
				default:
					return DefWindowProc(hwnd, message, wParam, lParam);
			}
		} else {
			recv = it->second;
			return recv->recvFunc(hwnd, message, wParam, lParam);
		}
	}
	HMODULE GetModuleFromAddress(void *address) {
		HMODULE instance = NULL;
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast<char *>(address), &instance);
		return instance;
	}
	void MessagePumpWin::MakeMessagePump(Task *InitTask, bool isTaskRunner) {
		char wndClassName[512];

		sprintf(wndClassName, "ThreadDispatchWin-%s", this->wndIdExtenstion);

		writeToLog(wndClassName);

		HRESULT errCode;
		HMODULE instance = NULL;

		instance = GetModuleFromAddress(&WndProc);

		WNDCLASSEX wx = {0};
		wx.cbSize = sizeof(WNDCLASSEX);
		wx.lpfnWndProc = WndProc;
		wx.hInstance = instance;
		wx.lpszClassName = wndClassName;

		SetLastError(S_OK);
		wndClassATOM = RegisterClassEx(&wx);
		errCode = HRESULT_FROM_WIN32(GetLastError());
		SetLastError(S_OK);

		errCode = HRESULT_FROM_WIN32(GetLastError());

		HWND win = CreateWindowEx(0, wndClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, instance, NULL);
		this->RegisterMessageHandler(new MessageReceiver(win, std::bind(ThreadMessageWndProc, _1, _2, _3, _4)));
		msgPumpWinInit = true;

		SendNotifyMessage(win, newTaskMessage, 0, (LPARAM)InitTask);

		this->StartMessageLoop(false);
	}
	void MessagePumpWin::MakeMessagePump(bool isTaskRunner) {
		char wndClassName[512];

		sprintf(wndClassName, "ThreadDispatchWin-%s", this->wndIdExtenstion);

		HRESULT errCode;
		HMODULE instance = NULL;

		instance = GetModuleFromAddress(&WndProc);

		WNDCLASSEX wx = {0};
		wx.cbSize = sizeof(WNDCLASSEX);
		wx.lpfnWndProc = WndProc;
		wx.hInstance = instance;
		wx.lpszClassName = wndClassName;

		SetLastError(S_OK);
		wndClassATOM = RegisterClassEx(&wx);
		errCode = HRESULT_FROM_WIN32(GetLastError());
		SetLastError(S_OK);

		errCode = HRESULT_FROM_WIN32(GetLastError());

		HWND win = CreateWindowEx(0, wndClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, instance, NULL);
		this->RegisterMessageHandler(new MessageReceiver(win, std::bind(ThreadMessageWndProc, _1, _2, _3, _4)));
		msgPumpWinInit = true;
		this->StartMessageLoop(isTaskRunner);
	}
	void MessagePumpWin::StartMessageLoop(bool isTaskRunner) {
		MSG msg;

		this->startComplete = true;
		this->postBlocker->Signal();

		while (GetMessage(&msg, NULL, 0, 0) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	void MessagePumpWin::PostMessageToThread(const char *thread, Task *task, bool isTaskRunner) {
		if (this->startComplete == false) { this->postBlocker->Wait(); }

		HWND threadMsgWindow;
		if (isTaskRunner) {
			char wndClassName[512];
			sprintf(wndClassName, "ThreadDispatchWin-%s", thread);

			threadMsgWindow = FindWindowEx(HWND_MESSAGE, NULL, wndClassName, NULL);
		} else {
			threadMsgWindow = FindWindowEx(HWND_MESSAGE, NULL, thread, NULL);
		}
		if (threadMsgWindow != 0) {
			SendNotifyMessage(threadMsgWindow, newTaskMessage, 0, (LPARAM)task);
		}
	}
}} // namespace base::threading
