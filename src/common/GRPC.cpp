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
			GitService::AsyncService svc;
			GitServiceAsyncImpl* asyncSvc = new GitServiceAsyncImpl(&svc);
			ServerBuilder builder;
			builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

			writeRefQueue = builder.AddCompletionQueue();
			receivePackQueue = builder.AddCompletionQueue();
			recvPackStreamQueue = builder.AddCompletionQueue();

			builder.RegisterService(&svc);
			this->server = builder.BuildAndStart();
			SetupAsyncHandler(asyncSvc);

			server->Wait();
		});
		serverThread.detach();
	}
	void GRPCServer::CreateGRPCServer() {
		unlink(SERVER_SOCKET);
		std::string address("unix:");
		address.append(SERVER_SOCKET);
		if (CreateUnixSocket(SERVER_SOCKET)) {
			CreateGRPCServerInternal(address);
		} else {
			LOG_MSG("failed to CreateUnixSocket");
			exit(0);
		}
	}
	void GRPCServer::CreateHTTPGRPCServer() {
		std::string serverAddr("0.0.0.0:9001");
		CreateGRPCServerInternal(serverAddr);
	}
	void GRPCServer::SetupAsyncHandler(GitServiceAsyncImpl* service) {
		service->HandleReceivePack(receivePackQueue.get());
		service->HandleWriteReference(writeRefQueue.get());
		service->HandleRecvPackStream(recvPackStreamQueue.get());
	}

}}