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

namespace nexus { namespace common {
	using namespace grpc;
	using namespace base::utils;
	SINGLETON_DEF(GRPCServer);
	void GRPCServer::CreateGRPCServer() {
		unlink(SERVER_SOCKET);
		std::thread serverThread([&] {
			std::string address("unix:");
			address.append(SERVER_SOCKET);
			if (CreateUnixSocket(SERVER_SOCKET)) {
				ServerBuilder builder;
				builder.AddListeningPort(address, grpc::InsecureServerCredentials());
				builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, 2);
				for (grpc::Service* s : this->services) {
					builder.RegisterService(s);
				}
				this->server = builder.BuildAndStart();
			} else {
				LOG_FROM_HERE_E("failed to CreateUnixSocket");
				exit(0);
			}
		});
		serverThread.detach();
	}
	void GRPCServer::RegisterService(grpc::Service* service) {
		this->services.push_back(service);
	}
}}