/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef RCVPAKSTRM
#define RCVPAKSTRM

#include <thread>
#include <base/Utils.h>
#include <base/common.h>
#include <common/GitServiceCommon.h>

namespace nexus { namespace git {
	using namespace grpc;
	class RecvPackStream {
		public:
			RecvPackStream(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) : svc(service), queue(cq) {}
			void HandlerThread();
			void StartHandlerThread();
		private:
			ServerCompletionQueue* queue;
			nexus::GitService::AsyncService* svc;
			std::unique_ptr<std::thread> requestHandler;
			class Request {
				public:
					Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq);
					bool ProcessRequest(RequestStatus status);
					RequestStatus status;
				private:
					void ReadMessage();
					void WriteResponse();

					ServerContext context;
					ReceivePackRequest msg;
					bool isRunning = true;
					ServerCompletionQueue* queue;
					nexus::GitService::AsyncService* svc;
					std::unique_ptr<ServerAsyncReaderWriter<GenericResponse, ReceivePackRequest>> sarw;
			};
	};
}}

#endif