/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>

#include <base/threading/dispatcher/DispatcherTypes.h>

#include <services/git/repository/GitRepo.h>
#include <services/git/requests/WriteReference.h>
#include <services/git/repository/RepositoryManager.h>

namespace nexus { namespace git {
	using namespace gitrpc::git;
	using namespace gitrpc::common;
	void WriteReference::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&WriteReference::HandlerThread, this)));
	}
	void WriteReference::HandlerThread() {
		bool ok = false;
		void* tag = nullptr;
		Request* req;
		new Request(svc, queue);
		while(this->queue->Next(&tag, &ok)) {
			req = static_cast<Request*>(tag);
			if (ok) { req->ProcessRequest(); }
		}
	}
	WriteReference::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
		LOG_MSG("queue write ref request")
		svc = service;
		queue = cq;
		status = RequestStatus::CONNECT;
		resp.reset(new ServerAsyncResponseWriter<GenericResponse>(&context));
		ProcessRequest();
	}
	bool WriteReference::Request::ProcessRequest() {
		nexus::GenericResponse r;
		if (status == RequestStatus::CONNECT) {
			LOG_MSG("connect");
			status = RequestStatus::READ;
			svc->RequestWriteReference(&context, &request, resp.get(), queue, queue, this);
		} else if (status == RequestStatus::READ) {
		 	new Request(svc, queue);
			Read();
		 	status = RequestStatus::FINISH;
			// resp->Finish(r, Status(StatusCode::OK, "Success"), this);
		// 	Read();
		} else {
			LOG_MSG("finish")
			GPR_ASSERT(status == RequestStatus::FINISH);
			delete this;
		// 	status = RequestStatus::FINISH;
		}
		return true;
	}
	void WriteReference::Request::Read() {
		LOG_MSG("read")
		int errCode;
		std::string err;
		git_oid objectID;
		std::future<std::string> result;
		result = RepoProxy::OpenRepo(request.reponame());
		result.wait();
		err = result.get();
		if (err != "success") {
			RepoProxy::CloseRepo();
			Write(err.c_str());
			return;
		}
		result = RepoProxy::CreateReference(request.refname(), request.refrev());
		result.wait();
		err = result.get();
		if (err != "success") {
			RepoProxy::CloseRepo();
			Write(err.c_str());
			return;
		}
		RepoProxy::CloseRepo();
		Write("Success");
	}
	void WriteReference::Request::Write(const char* msg) {
		LOG_ARGS("%s", msg)
		nexus::GenericResponse r;
		r.set_errormessage(msg);
		if (strcmp(msg, "Success") == 0) {
			r.set_success(true);
			resp->Finish(r, Status(StatusCode::OK, "Success"), this);
		} else {
			r.set_success(false);
			resp->Finish(r, Status(StatusCode::INTERNAL, msg), this);
		}
	}
}}