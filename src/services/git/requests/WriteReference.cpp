/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>
#include <services/git/requests/WriteReference.h>

namespace nexus { namespace git {
	void WriteReference::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&WriteReference::HandlerThread, this)));
	}
	void WriteReference::HandlerThread() {
		bool ok = false;
		void* tag = nullptr;
		WriteReference::Request* req;
		new WriteReference::Request(svc, queue);
		while(this->queue->Next(&tag, &ok)) {
			if (ok) {
				static_cast<WriteReference::Request*>(tag)->ProcessRequest();
			} else {
				LOG_ARGS("ok = false %p", req)
			}
		}
	}
	WriteReference::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
		LOG_MSG("queue write ref request")
		svc = service;
		queue = cq;
		status = RequestStatus::CONNECT;
		context.AsyncNotifyWhenDone(this);
		resp.reset(new ServerAsyncResponseWriter<GenericResponse>(&context));
		ProcessRequest();
	}
	bool WriteReference::Request::ProcessRequest() {
		if (status == RequestStatus::CONNECT) {
			LOG_MSG("connect");
			status = RequestStatus::READ;
			svc->RequestWriteReference(&context, &request, resp.get(), queue, queue, this);
		} else if (status == RequestStatus::READ) {
			LOG_ARGS("refname %s", request.refname().c_str());
			new Request(svc, queue);
			nexus::GenericResponse r;
			r.set_errormessage("success");
			r.set_success(true);
			resp->Finish(r, Status(StatusCode::OK, "Success"), this);
			status = RequestStatus::DONE;
		}
		return true;
	}
}}