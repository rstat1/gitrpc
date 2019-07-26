/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <services/git/repository/GitRepo.h>
#include <services/git/requests/RecvPackStream.h>
#include <services/git/repository/RepositoryManager.h>

namespace nexus { namespace git {
	using namespace gitrpc::git;
	using namespace base::threading;
	using namespace gitrpc::common;
    void RecvPackStream::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&RecvPackStream::HandlerThread, this)));
	}
    void RecvPackStream::HandlerThread() {
		bool ok = false;
		void* tag = nullptr;
		RequestStatus last;
        auto req = new RecvPackStream::Request(svc, queue);
		while(this->queue->Next(&tag, &ok)) {
			if (ok) {
				req->ProcessRequest(static_cast<RequestStatus>(reinterpret_cast<size_t>(tag)));
			} else {
				LOG_MSG("queue new request")
				if (req != nullptr) { req->FinishRequest(); }
				//delete req;
				req = new RecvPackStream::Request(svc, queue);
			}
        }
    }
    RecvPackStream::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
        LOG_MSG("queue rp stream")
        queue = cq;
		svc = service;
		status = RequestStatus::CONNECT;
        context.AsyncNotifyWhenDone(reinterpret_cast<void*>(RequestStatus::FINISH));
        sarw.reset(new ServerAsyncReaderWriter<GenericResponse, ReceivePackRequest>(&context));
        svc->RequestReceivePackStream(&context, sarw.get(), queue, queue, reinterpret_cast<void*>(RequestStatus::CONNECT));
    }
    bool RecvPackStream::Request::ProcessRequest(RequestStatus status) {
		// LOG_MSG("rp stream process");
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

				// if (requestFailed) {
				// 	sarw->Finish(Status(StatusCode::INTERNAL, failureReason), reinterpret_cast<void*>(RequestStatus::FINISH));
				// } else {
				// 	sarw->Finish(Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
				// }
				break;
			case RequestStatus::FINISH:
				LOG_MSG("finish")
				isRunning = false;
				FinishRequest();
				// delete this;
				break;
		}
        return false;
    }
	void RecvPackStream::Request::FinishRequest() {
		std::string ret;
		nexus::GenericResponse r;
		if (repoOpen) {
			LOG_MSG("finish request");
			auto commitRet = RepoProxy::PackCommit();
			commitRet.wait();
			ret = commitRet.get();
			if (ret != "success") {
				resp = Common::Response(ret.c_str(), false, StatusCode::INTERNAL);
				r.set_errormessage(resp->errorMessage);
				r.set_success(resp->success);
				failureReason = resp->errorMessage;
				requestFailed = true;
				WriteOptions wopts;
				wopts = wopts.set_last_message();
				sarw->Write(r, wopts, reinterpret_cast<void*>(RequestStatus::DONE));
			} else {
				sarw->Finish(Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
			}
			RepoProxy::CloseRepo();
			delete this;
		} else {
			LOG_MSG("hmm...")
		}
		// if (current != nullptr) {
		// 	nexus::GenericResponse r;
		// 	const char* err = current->PackCommit(nullptr);
		// 	if (err != "success") {
		// 		LOG_REL_A("failed to commit pack data: %s", err)
		// 		resp = Common::Response(err, false, StatusCode::INTERNAL);
		// 		r.set_errormessage(resp->errorMessage);
		// 		r.set_success(resp->success);
		// 		failureReason = resp->errorMessage;
		// 		requestFailed = true;
		// 		sarw->Write(r, reinterpret_cast<void*>(RequestStatus::DONE));
		// 	}
		// 	delete current;
		// }
	}
	void RecvPackStream::Request::WriteResponse() {
		nexus::GenericResponse r;
		WriteOptions wopts;
		wopts = wopts.set_last_message();
		r.set_errormessage("Success");
		r.set_success(true);
		sarw->Write(r, reinterpret_cast<void*>(RequestStatus::WRITE));

		// if (resp == nullptr) {
		// 	if (current != nullptr) {
		// 		const char* err = current->PackCommit(nullptr);
		// 		if (err != "success") {
		// 			LOG_REL_A("failed to commit pack data: %s", err)
		// 			resp = Common::Response(err, false, StatusCode::INTERNAL);
		// 			r.set_errormessage(resp->errorMessage);
		// 			r.set_success(resp->success);
		// 			failureReason = resp->errorMessage;
		// 			requestFailed = true;
		// 			sarw->Write(r, reinterpret_cast<void*>(RequestStatus::DONE));
		// 			delete current;
		// 		} else {
		// 		}
		// 	} else {
		// 		sarw->Write(r, reinterpret_cast<void*>(RequestStatus::WRITE));
		// 	}
		// } else {
		// 	r.set_errormessage(resp->errorMessage);
		// 	r.set_success(resp->success);
		// 	sarw->Write(r, reinterpret_cast<void*>(RequestStatus::DONE));
		// }
		// LOG_MSG("write")
	}
	void RecvPackStream::Request::ReadMessage() {
		std::string err;
		git_transfer_progress stats;
		if (isRunning) {
			sarw->Read(&msg, reinterpret_cast<void*>(RequestStatus::READ));
			if (msg.data().size() > 0) {
				// LOG_ARGS("read %i", msg.data().size())
				if (repoOpen == false) {
					auto openResult = RepoProxy::OpenRepo(msg.reponame());
					openResult.wait();
					err = openResult.get();
					if (err != "success") {
						LOG_REL_A("failed to open repo: %s", err.c_str())
						resp = Common::Response(err.c_str(), false, StatusCode::INTERNAL);
						return;
					} else {
						repoOpen = true;
					}
				}
				auto packAppendRet = RepoProxy::PackAppend(msg.data().data(), msg.data().size());
				packAppendRet.wait();
				err = packAppendRet.get();
				if (err != "success") {
					LOG_REL_A("failed to write pack data: %s", err.c_str())
					resp = Common::Response(err.c_str(), false, StatusCode::INTERNAL);
					return;
				}
			}
		} else {
			LOG_MSG("not running anymroe")
		}
	}
}}