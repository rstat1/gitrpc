/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef RPOSTATCHNG
#define RPOSTATCHNG

#include <future>
#include <thread>

#include <common/GitServiceCommon.h>

namespace nexus { namespace git {
	using namespace grpc;
	class RepoStateChange {
	public:
		RepoStateChange(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) : svc(service), queue(cq) {}
		void HandlerThread();
		void StartHandlerThread();

	private:
		ServerCompletionQueue* queue;
		nexus::GitService::AsyncService* svc;
		std::unique_ptr<std::thread> requestHandler;
		class Request {
		public:
			Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq);
			bool ProcessRequest();
			void FinishRequest();
			// ~Request(){ LOG_MSG("goodbye!") }

			RequestStatus currentStatus;

		private:
			void Read();
			void Write(const char* msg);

			ServerContext context;
			RequestStatus status;
			ServerCompletionQueue* queue;
			RepoStateChangeRequest request;
			nexus::GitService::AsyncService* svc;
			std::unique_ptr<ServerAsyncResponseWriter<GenericResponse>> resp;
		};
	};
}} // namespace nexus::git

#endif