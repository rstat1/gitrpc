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
#include <common/GRPC.h>
#include <grpcpp/server.h>
#include <grpc++/grpc++.h>
#include <services/git/GitServiceImpl.h>

namespace nexus { namespace common {
	using namespace grpc;
	using namespace base::utils;
	SINGLETON_DEF(GRPCServer);
	void GRPCServer::CreateGRPCServer() {
		unlink(SERVER_SOCKET);
		std::thread serverThread([&] {
			nexus::git::GitServiceImpl* gitSvc = new nexus::git::GitServiceImpl();
			gitSvc->InitGitService();
			std::string address("unix:");
			address.append(SERVER_SOCKET);
			if (CreateUnixSocket(SERVER_SOCKET)) {
				ServerBuilder builder;
				builder.AddListeningPort(address, grpc::InsecureServerCredentials());
				builder.RegisterService(gitSvc);
				this->server = builder.BuildAndStart();
				server->Wait();
			} else {
				LOG_MSG("failed to CreateUnixSocket");
				exit(0);
			}
		});
		serverThread.detach();
	}
	void GRPCServer::CreateHTTPGRPCServer() {
		std::thread serverThread([&] {
			std::string serverAddr("0.0.0.0:9001");
			nexus::git::GitServiceImpl* gitSvc = new nexus::git::GitServiceImpl();
			gitSvc->InitGitService();
			ServerBuilder builder;
			builder.AddListeningPort(serverAddr, grpc::InsecureServerCredentials());
			// builder.experimental().SetInterceptorCreators(std::move(interceptor_creators));
			builder.RegisterService(gitSvc);
			this->server = builder.BuildAndStart();
			server->Wait();
		});
		serverThread.detach();
	}
	void GRPCServer::RegisterService(grpc::Service* service) {
		this->services.push_back(service);
	}
}}