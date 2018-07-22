/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GRPCSERV
#define GRPCSERV

#include <vector>
#include <base/common.h>
#include <grpcpp/server_builder.h>
#include <common/GitServiceCommon.h>

namespace nexus { namespace common {
	using namespace grpc;
	class GRPCServer {
		public:
			void CreateGRPCServer();
			void RegisterService(grpc::Service* service);

		private:
			std::unique_ptr<Server> server;
			std::vector<grpc::Service*> services;
		SINGLETON(GRPCServer);
	};
}}

#endif