/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef IACCHAN
#define IACCHAN

#include <base/common.h>
#include <base/ipc/IPCTypes.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
#include <functional>
#include <string>

using namespace base::threading;

namespace base { namespace ipc {

	class BASEAPI PlatformChannelPair {
	public:
		PlatformChannelPair();
		int GetClientHandle() { return pipes[1]; }
		// TaskResult* ConnectToPipe();
		// ChannelInfo* CreatePipe(const char* channelName);
		// TaskResult* WriteMessage(void* buffer);
		// TaskResult* ReadMessage();
	private:
#if defined(OS_WIN)
		HANDLE completionPort;
		OVERLAPPED connectCtx;
		HANDLE pipe;
#else
		int pipes[2];
		int epollFD;
#endif
	};
}} // namespace base::ipc

#endif