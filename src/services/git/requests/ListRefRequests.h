/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef LSTREFREQ
#define LSTREFREQ

#include <thread>

#include <common/GitServiceCommon.h>

namespace nexus { namespace git {
	using namespace grpc;
	class ListRefRequests {
	public:
		ListRefRequests(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) : svc(service), queue(cq) {}
		void HandlerThread();
		void StartHandlerThread();

	private:
		ServerCompletionQueue* queue;
		nexus::GitService::AsyncService* svc;
		std::unique_ptr<std::thread> requestHandler;
		class Request {
		public:
			Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq);
			void ProcessRequest();
			~Request(){LOG_MSG("goodbye!")}

			RequestStatus status;

		private:
			void Read();
			void Write();

			ServerContext context;
			ListRefsRequest request;
			ServerCompletionQueue* queue;
			nexus::GitService::AsyncService* svc;
			std::unique_ptr<ServerAsyncResponseWriter<ListRefsResponse>> resp;
		};
	};
}} // namespace nexus::git

#endif