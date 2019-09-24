/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GSAIMPL
#define GSAIMPL

#include <base/common.h>
#include <common/GitServiceCommon.h>

#define HANDLER_DEF(name) void Handle##name(ServerCompletionQueue* queue)
#define HANDLER(name) void GitServiceAsyncImpl::Handle##name(ServerCompletionQueue* queue) { (new name(gitService, queue))->StartHandlerThread(); }

namespace nexus { namespace git {
	using namespace grpc;
	class GitServiceAsyncImpl {
		public:
			GitServiceAsyncImpl(GitService::AsyncService* service) : gitService(service) {}
			HANDLER_DEF(ReceivePack);
			HANDLER_DEF(WriteReference);
			HANDLER_DEF(RecvPackStream);
			HANDLER_DEF(RepoStateChange);
		private:
			GitService::AsyncService* gitService;
	};
}}

#endif