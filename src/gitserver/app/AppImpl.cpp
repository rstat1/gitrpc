/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <gitserver/app/AppImpl.h>
#include <gitserver/GitServiceImpl.h>

namespace nexus {
	using namespace nexus::git;
    void AppImpl::Main() {
		LOG_FROM_HERE_E("In AppImpl::AppMain");
		GitServiceImpl::Get()->CreateGRPCServer();
    }
}