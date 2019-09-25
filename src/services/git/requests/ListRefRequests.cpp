/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <services/git/requests/ListRefRequests.h>

namespace nexus { namespace git {
	void ListRefRequests::HandlerThread() {
	}
	void ListRefRequests::StartHandlerThread() {
	}
	ListRefRequests::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
		LOG_MSG("new ListRefRequest request")
		queue = cq;
		svc = service;
		status = RequestStatus::CONNECT;
		resp.reset(new ServerAsyncResponseWriter<ListRefsResponse>(&context));
		ProcessRequest();
	}
	void ListRefRequests::Request::ProcessRequest() {
		nexus::ListRefsResponse refs;
		if (status == RequestStatus::CONNECT) {
			LOG_MSG("connect");
			status = RequestStatus::READ;
			svc->RequestListRefs(&context, &request, resp.get(), queue, queue, this);
		} else if (status == RequestStatus::READ) {
			new Request(svc, queue);
			Read();
			status = RequestStatus::FINISH;
		} else {
			LOG_MSG("finish")
			GPR_ASSERT(status == RequestStatus::FINISH);
			delete this;
		}
	}
	void ListRefRequests::Request::Read() {
		
	}
	void ListRefRequests::Request::Write() {
	}
}} // namespace nexus::git