/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef IPCCHAN
#define IPCCHAN

#include <base/base_exports.h>
#include <base/ipc/PlatformPipe.h>
#include <functional>
#include <string>

// using namespace base::threading::IO;

namespace base { namespace ipc {
	class BASEAPI IPCChannel {
	public:
		IPCChannel();
		void WriteMessage(void *buffer);
		int GetClientHandle() { return channelBacking->GetClientHandle(); }

	private:
		PlatformChannelPair *channelBacking;
	};
}} // namespace base::ipc

#endif