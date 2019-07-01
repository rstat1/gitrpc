/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <common/GRPC.h>
#include <app/AppImpl.h>
#include <services/git/GitServiceImpl.h>

namespace nexus {
	using namespace nexus::git;
	using namespace nexus::common;
    void AppImpl::Main() {
		LOG_MSG("In AppImpl::AppMain");
		GRPCServer::Get()->CreateGRPCServer();
    }
}