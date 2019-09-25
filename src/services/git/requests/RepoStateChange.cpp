/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <services/git/repository/RepoProxy.h>
#include <services/git/requests/RepoStateChange.h>

namespace nexus { namespace git {
	using namespace gitrpc::git;
	using namespace gitrpc::common;
	void RepoStateChange::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&RepoStateChange::HandlerThread, this)));
	}
	void RepoStateChange::HandlerThread() {
		bool ok = false;
		void* tag = nullptr;
		Request* req;
		new Request(svc, queue);
		while (this->queue->Next(&tag, &ok)) {
			req = static_cast<Request*>(tag);
			if (ok) { req->ProcessRequest(); }
		}
	}
	RepoStateChange::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
		// LOG_MSG("new RepoStateChange")
		svc = service;
		queue = cq;
		status = RequestStatus::CONNECT;
		resp.reset(new ServerAsyncResponseWriter<GenericResponse>(&context));
		ProcessRequest();
	}
	bool RepoStateChange::Request::ProcessRequest() {
		nexus::GenericResponse r;
		if (status == RequestStatus::CONNECT) {
			// LOG_MSG("connect");
			status = RequestStatus::READ;
			svc->RequestChangeRepositoryState(&context, &request, resp.get(), queue, queue, this);
		} else if (status == RequestStatus::READ) {
			new Request(svc, queue);
			Read();
			status = RequestStatus::FINISH;
		} else {
			// LOG_MSG("finish")
			GPR_ASSERT(status == RequestStatus::FINISH);
			delete this;
		}
		return true;
	}
	void RepoStateChange::Request::Read() {
		int errCode;
		std::string err;
		std::future<std::string> result;
		if (request.newrepostate()) {
			LOG_MSG("open repo")
			result = RepoProxy::Get()->OpenRepo(request.reponame());
			result.wait();
			err = result.get();
			Write(err.c_str());
		} else {
			LOG_MSG("close repo")
			RepoProxy::Get()->CloseRepo();
			Write("success");
		}
	}
	void RepoStateChange::Request::Write(const char* msg) {
		nexus::GenericResponse r;
		r.set_errormessage(msg);
		if (strcmp(msg, "success") == 0) {
			r.set_success(true);
			resp->Finish(r, Status(StatusCode::OK, "Success"), this);
		} else {
			LOG_ARGS("error %s", msg)
			r.set_success(false);
			resp->Finish(r, Status(StatusCode::INTERNAL, msg), this);
		}
	}
}} // namespace nexus::git