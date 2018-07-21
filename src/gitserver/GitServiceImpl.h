/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GSIMPL
#define GSIMPL

#include <base/common.h>
#include <grpcpp/server.h>
#include <common/GitServiceCommon.h>

namespace nexus { namespace git {
	using namespace grpc;
	using namespace google::protobuf;
	class GitServiceImpl : public GitService::Service {
		public:
			void CreateGRPCServer();
			Status ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response);
			Status ListRefsForClone(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response);
			Status UploadPack(ServerContext* context, const UploadPackRequest* request, UploadPackResponse* response);
			Status ReceivePack(ServerContext* context, const ReceivePackRequest* request, Empty* response);

		private:
			std::unique_ptr<Server> server;

		SINGLETON(GitServiceImpl);
	};
}}

#endif