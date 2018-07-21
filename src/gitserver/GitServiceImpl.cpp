/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>
#include <sys/un.h>
#include <sys/socket.h>
#include <base/Utils.h>
#include <grpcpp/server_builder.h>
#include <gitserver/GitServiceImpl.h>

namespace nexus { namespace git {
	using namespace base::utils;
	SINGLETON_DEF(GitServiceImpl);
	void GitServiceImpl::CreateGRPCServer() {
		unlink(SERVER_SOCKET);
		std::thread serverThread([&] {
			std::string address("unix:");
			address.append(SERVER_SOCKET);
			if (CreateUnixSocket(address.c_str())) {
				ServerBuilder builder;
				builder.AddListeningPort(address, grpc::InsecureServerCredentials());
				builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, 2);
				builder.RegisterService(this);
				this->server = builder.BuildAndStart();
			}
		});
		serverThread.detach();
	}
	Status GitServiceImpl::ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_FROM_HERE_E("ListKnowRefs request")
		return Status::OK;
	}
	Status GitServiceImpl::ListRefsForClone(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_FROM_HERE_E("ListKnowRefsForClone request")
		return Status::OK;
	}
	Status GitServiceImpl::UploadPack(ServerContext* context, const UploadPackRequest* request, UploadPackResponse* response) {
		LOG_FROM_HERE_E("UploadPack request")
		return Status::OK;
	}
	Status GitServiceImpl::ReceivePack(ServerContext *context, const ReceivePackRequest *request, Empty *response) {
		LOG_FROM_HERE_E("ReceivePack request")
		return Status::OK;
	}
}}