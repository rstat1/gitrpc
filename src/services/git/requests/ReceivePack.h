/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef RECVPAK
#define RECVPAK

#include <thread>
#include <base/Utils.h>
#include <base/common.h>
#include <common/GitServiceCommon.h>

namespace nexus { namespace git {
    using namespace grpc;
	#define STATUS_TAG(status) reinterpret_cast<void*>(RequestStatus::##status)
    class ReceivePack {
		public:
			ReceivePack(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) : svc(service), queue(cq) {}
			void HandlerThread();
            void StartHandlerThread();
		private:
			class Request {
				public:
					Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq);
					bool ProcessRequest();
				private:
					RequestStatus status;
					ServerContext context;
					ServerCompletionQueue* queue;
					nexus::ReceivePackRequest request;
					nexus::GitService::AsyncService* svc;
					std::unique_ptr<ServerAsyncResponseWriter<GenericResponse>> resp;
			};
			ServerCompletionQueue* queue;
			nexus::GitService::AsyncService* svc;
			std::unique_ptr<std::thread> requestHandler;

	};
}}

#endif