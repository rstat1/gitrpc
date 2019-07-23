/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef WRTREF
#define WRTREF

#include <thread>
#include <base/Utils.h>
#include <base/common.h>
#include <common/GitServiceCommon.h>

namespace nexus { namespace git {
	using namespace grpc;
	class WriteReference {
		public:
			WriteReference(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) : svc(service), queue(cq) {}
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
				private:
					RequestStatus status;
					ServerContext context;
					ServerCompletionQueue* queue;
					WriteReferenceRequest request;
					nexus::GitService::AsyncService* svc;
					std::unique_ptr<ServerAsyncResponseWriter<GenericResponse>> resp;
			};
	};
}}

#endif