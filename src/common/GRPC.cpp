/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>
#include <sys/un.h>
#include <sys/socket.h>

#include <grpcpp/server.h>
#include <grpc++/grpc++.h>
#include <grpcpp/resource_quota.h>

#include <base/Utils.h>
#include <common/GRPC.h>
#include <services/git/GitServiceImpl.h>
#include <services/git/GitServiceAsyncImpl.h>

namespace nexus { namespace common {
	using namespace grpc;
	using namespace nexus::git;
	using namespace base::utils;
	SINGLETON_DEF(GRPCServer);
	void GRPCServer::CreateGRPCServerInternal(std::string addr) {
		std::thread serverThread([&] {
			LOG_ARGS("starting grpc server at address %s", addr.c_str());

			GitService::AsyncService svc;
			GitServiceAsyncImpl* asyncSvc = new GitServiceAsyncImpl(&svc);
			ServerBuilder builder;
			builder.AddListeningPort(currentAddress, grpc::InsecureServerCredentials());

			writeRefQueue = builder.AddCompletionQueue();
			receivePackQueue = builder.AddCompletionQueue();
			recvPackStreamQueue = builder.AddCompletionQueue();
			repoStateStreamQueue = builder.AddCompletionQueue();

			builder.RegisterService(&svc);
			this->server = builder.BuildAndStart();
			SetupAsyncHandler(asyncSvc);

			server->Wait();
		});
		serverThread.detach();
	}
	void GRPCServer::CreateGRPCServer() {
		unlink(SERVER_SOCKET);
		currentAddress.append("unix:");
		currentAddress.append(SERVER_SOCKET);
		if (CreateUnixSocket(SERVER_SOCKET)) {
			CreateGRPCServerInternal(currentAddress);
		} else {
			LOG_MSG("failed to CreateUnixSocket");
			exit(0);
		}
	}
	void GRPCServer::CreateHTTPGRPCServer() {
		currentAddress.append("0.0.0.0:9001");
		CreateGRPCServerInternal(currentAddress);
	}
	void GRPCServer::SetupAsyncHandler(GitServiceAsyncImpl* service) {
		service->HandleWriteReference(writeRefQueue.get());
		service->HandleRecvPackStream(recvPackStreamQueue.get());
		service->HandleRepoStateChange(repoStateStreamQueue.get());
	}

}}