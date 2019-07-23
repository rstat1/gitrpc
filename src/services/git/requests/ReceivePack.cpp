/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <grpcpp/alarm.h>
#include <services/git/requests/ReceivePack.h>

namespace nexus { namespace git {
	void ReceivePack::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&ReceivePack::HandlerThread, this)));
	}
	void ReceivePack::HandlerThread() {
		bool ok = false;
		grpc::Alarm alarm;
		void* tag = nullptr;
		bool requeue = false;
		new ReceivePack::Request(svc, queue);
		while(this->queue->Next(&tag, &ok)) {
			if (ok) { static_cast<ReceivePack::Request*>(tag)->ProcessRequest(); }
		}
	}
	ReceivePack::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
		LOG_MSG("queue rp unary")
		queue = cq;
		svc = service;
		status = RequestStatus::CONNECT;
		resp.reset(new ServerAsyncResponseWriter<GenericResponse>(&context));
		ProcessRequest();
	}
	bool ReceivePack::Request::ProcessRequest() {
		nexus::GenericResponse r;
		if (status == RequestStatus::CONNECT) {
			status = RequestStatus::READ;
			svc->RequestReceivePack(&context, &request, resp.get(), queue, queue, this);
		} else if (status == RequestStatus::READ) {
			new Request(svc, queue);
			r.set_errormessage("success");
			r.set_success(true);
			resp->Finish(r, Status(StatusCode::OK, "Success"), this);
			status = RequestStatus::DONE;
		} else if (status == RequestStatus::DONE) {
			delete this;
		}
		return true;
	}
}}