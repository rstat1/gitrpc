/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>
#include <services/git/GitServiceAsyncImpl.h>

#include <services/git/requests/ReceivePack.h>
#include <services/git/requests/WriteReference.h>
#include <services/git/requests/RecvPackStream.h>

namespace nexus { namespace git {
	HANDLER(ReceivePack)
	HANDLER(WriteReference)
	HANDLER(RecvPackStream)
}}