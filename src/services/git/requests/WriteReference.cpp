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
			new Request(svc, queue);
			Read();
			status = RequestStatus::DONE;
		} else {
			LOG_MSG("how about here?")
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
			WriteError(err.c_str());
			return;
		}

		result = RepoProxy::CreateReference(request.refname(), request.refrev());
		err = result.get();
		if (err != "success") {
			RepoProxy::CloseRepo();
			WriteError(err.c_str());
			return;
		}
		RepoProxy::CloseRepo();
		Write();
	}
	void WriteReference::Request::Write() {
		LOG_MSG("write ref write resp")
		nexus::GenericResponse r;
		r.set_errormessage("Success");
		r.set_success(true);
		resp->Finish(r, Status(StatusCode::OK, "Success"), this);
	}
	void WriteReference::Request::WriteError(const char* error) {
		LOG_MSG("write ref write err resp")
		nexus::GenericResponse r;
		r.set_errormessage(error);
		r.set_success(false);
		LOG_REL_A("error writing reference %s", error)
		resp->Finish(r, Status(StatusCode::INTERNAL, error), this);
		delete this;
	}
}}