/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <services/git/GitRepo.h>
#include <services/git/requests/RecvPackStream.h>

namespace nexus { namespace git {
	using namespace gitrpc::common;
    void RecvPackStream::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&RecvPackStream::HandlerThread, this)));
	}
    void RecvPackStream::HandlerThread() {
		LOG_MSG("init libgit");
		bool ok = false;
		void* tag = nullptr;
		bool requeue = false;
		RequestStatus last;
        auto req = new RecvPackStream::Request(svc, queue);
		while(this->queue->Next(&tag, &ok)) {
			if (ok) {
				last = req->status;
				req->ProcessRequest(static_cast<RequestStatus>(reinterpret_cast<size_t>(tag)));
			} else {
				delete req;
				req = new RecvPackStream::Request(svc, queue);
			}
        }
    }
    RecvPackStream::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
        LOG_MSG("queue rp stream")
        queue = cq;
		svc = service;
		status = RequestStatus::CONNECT;
        context.AsyncNotifyWhenDone(reinterpret_cast<void*>(RequestStatus::DONE));
        sarw.reset(new ServerAsyncReaderWriter<GenericResponse, ReceivePackRequest>(&context));
        svc->RequestReceivePackStream(&context, sarw.get(), queue, queue, reinterpret_cast<void*>(RequestStatus::CONNECT));
    }
    bool RecvPackStream::Request::ProcessRequest(RequestStatus status) {
		LOG_MSG("rp stream process");
		switch(status) {
			case RequestStatus::CONNECT:
				ReadMessage();
				break;
			case RequestStatus::READ:
				WriteResponse();
				break;
			case RequestStatus::WRITE:
				ReadMessage();
				break;
			case RequestStatus::DONE:
				LOG_MSG("done")
				isRunning = false;
				sarw->Finish(resp->requestStatus, reinterpret_cast<void*>(RequestStatus::FINISH));
				delete resp;
				break;
			case RequestStatus::FINISH:
				LOG_MSG("finish")
				// delete this;
				break;
		}
        return false;
    }
	void RecvPackStream::Request::FinishRequest() {
		sarw->Finish(Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
	}
	void RecvPackStream::Request::WriteResponse() {
		nexus::GenericResponse r;

		r.set_errormessage("Success");
		r.set_success(true);

		if (resp == nullptr) {
			if (current != nullptr) {
				const char* err = current->PackCommit(nullptr);
				if (err != "success") {
					LOG_REL_A("failed to commit pack data: %s", err)
					resp = Common::Response(err, false, StatusCode::INTERNAL);
					sarw->Write(*resp->response, reinterpret_cast<void*>(RequestStatus::DONE));
					delete current;
				} else {
					sarw->Write(r, reinterpret_cast<void*>(RequestStatus::WRITE));
				}
				delete current;
			} else {
				sarw->Write(r, reinterpret_cast<void*>(RequestStatus::WRITE));
			}
		} else {
			sarw->Write(*resp->response, reinterpret_cast<void*>(RequestStatus::DONE));
		}
		LOG_MSG("write")
	}
	void RecvPackStream::Request::ReadMessage() {
		const char* err;
		git_transfer_progress stats;
		if (isRunning) {
			sarw->Read(&msg, reinterpret_cast<void*>(RequestStatus::READ));

			if (msg.data().size() > 0) {
				LOG_ARGS("read %i", msg.data().size())
				current = new GitRepo(msg.reponame());
				err = current->Open(true);
				if (err != "success") {
					LOG_REL_A("failed to open repo: %s", err)
					resp = Common::Response(err, false, StatusCode::INTERNAL);
					delete current;
					return;
				}
				err = current->PackAppend(msg.data().data(), msg.data().size(), &stats);
				if (err != "success") {
					LOG_REL_A("failed to write pack data: %s", err)
					resp = Common::Response(err, false, StatusCode::INTERNAL);
					delete current;
					return;
				}
			}
		}
	}
}}