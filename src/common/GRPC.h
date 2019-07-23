/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GRPCSERV
#define GRPCSERV

#include <base/common.h>
#include <grpcpp/server_builder.h>
#include <common/GitServiceCommon.h>

namespace nexus::git {
	class GitServiceAsyncImpl;
}

namespace nexus { namespace common {
	using namespace grpc;

	class GRPCServer {
		public:
			void CreateGRPCServer();
			void CreateHTTPGRPCServer();
			void RegisterService(grpc::Service* service);

		private:
			void CreateGRPCServerInternal(std::string addr);
			void SetupAsyncHandler(nexus::git::GitServiceAsyncImpl* service);

			ServerContext context;
			std::unique_ptr<Server> server;
			nexus::GitService::AsyncService* svc;
			std::unique_ptr<ServerCompletionQueue> writeRefQueue;
			std::unique_ptr<ServerCompletionQueue> receivePackQueue;
			std::unique_ptr<ServerCompletionQueue> recvPackStreamQueue;
			std::unique_ptr<ServerAsyncReader<GenericResponse, ReceivePackRequest>> reader;

		SINGLETON(GRPCServer);
	};
}}

#endif