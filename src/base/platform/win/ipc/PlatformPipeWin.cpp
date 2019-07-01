/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <cwchar>
#include <string>
#include <stdio.h>
#include <cassert>
#include <Windows.h>
#include <base/Utils.h>

#include <base/ipc/PlatformPipe.h>

using namespace base::utils;
using namespace base::threading;

namespace base { namespace ipc 
{	
	std::string GeneratePipeName(const char* channelName)
	{
		char buffer[128];
		snprintf(buffer, 128, "\\\\.\\pipe\\%s.%s", channelName, std::to_string(GetCurrentProcessId()).c_str());
		return std::string(buffer);
	}
	ChannelInfo* PlatformPipe::CreatePipe(const char* channelName)
	{
		HRESULT errCode;
		std::string pipeName = GeneratePipeName(channelName);
		const DWORD pipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_REJECT_REMOTE_CLIENTS;
		const DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
		DWORD openMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;

		Log("All", "Pipe Name: %s", pipeName.c_str());
		memset(&connectCtx, 0, sizeof(connectCtx));
		connectCtx.hEvent = CreateEventA(NULL, TRUE, TRUE, NULL);

		pipe = CreateNamedPipeA(pipeName.c_str(), openMode, pipeMode, 1, 4096, 4096, 5000, nullptr);
		
		Log("All", "PlatformPipe::CreatePipe(): %d", pipe);

		completionPort = CreateIoCompletionPort(pipe, NULL, reinterpret_cast<ULONG_PTR>(this), 1);

		return new ChannelInfo();
	}
	TaskResult* PlatformPipe::ConnectToPipe()
	{
		ConnectNamedPipe(pipe, &connectCtx);
		DWORD ret = WaitForSingleObject(connectCtx.hEvent, INFINITE);
		if (ret == WAIT_OBJECT_0) { return new TaskResult(true); }
		else { return new TaskResult(false); }
	}	
	TaskResult* PlatformPipe::WriteMessage(void* buffer)
	{
		OVERLAPPED writeCtx;
		memset(&writeCtx, 0, sizeof(writeCtx));
		writeCtx.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

		return nullptr;
	}
	TaskResult* PlatformPipe::ReadMessage()
	{
		OVERLAPPED readCtx;
		memset(&readCtx, 0, sizeof(readCtx));
		readCtx.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

		std::string* message = new std::string();
		message->assign(4096, 0);
		DWORD bytesRead;
		
		BOOL success = ReadFile(pipe, &(*message)[0], static_cast<DWORD>(4096), NULL, &readCtx);
		if (success)
		{
			success = PostQueuedCompletionStatus(completionPort, 4096, reinterpret_cast<ULONG_PTR>(this), &readCtx);
			if (!success) { Log("All", "PostQueuedCompletionStatus(): %s", GetLastErrorStdStr().c_str()); }
		}
		else { WaitForSingleObject(readCtx.hEvent, INFINITE); }

		TaskResult* res = new TaskResult();
		res->result = std::move(message);

		return res;
	}
}}