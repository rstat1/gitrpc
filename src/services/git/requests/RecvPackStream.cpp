/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <grpc++/alarm.h>

#include <base/Utils.h>

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
		RequestStatus status;
        auto req = new Request(svc, queue);
		while(this->queue->Next(&tag, &ok)) {
			status = static_cast<RequestStatus>(reinterpret_cast<size_t>(tag));
			if (ok) { req->ProcessRequest(status); }
			if (status == RequestStatus::FINISH) {
				delete req;
				req = new Request(svc, queue);
			}
        }
    }
    RecvPackStream::Request::Request(nexus::GitService::AsyncService* service, ServerCompletionQueue* cq) {
        LOG_ARGS("new rp stream %p", (void*)this)
        queue = cq;
		svc = service;
		currentStatus = RequestStatus::CONNECT;
		id = base::utils::GenerateRandomString(16);
        // context.AsyncNotifyWhenDone(reinterpret_cast<void*>(RequestStatus::DONE));
        sarw.reset(new ServerAsyncReaderWriter<GenericResponse, ReceivePackRequest>(&context));
        svc->RequestReceivePackStream(&context, sarw.get(), queue, queue, reinterpret_cast<void*>(RequestStatus::CONNECT));
    }
    bool RecvPackStream::Request::ProcessRequest(RequestStatus status) {
		currentStatus = status;
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
				LOG_ARGS("done %s", id.c_str())
				isRunning = false;
				if (repoOpen) { FinishRequest(); }
				// if (requestFailed) {
				// 	sarw->Finish(Status(StatusCode::INTERNAL, failureReason), reinterpret_cast<void*>(RequestStatus::FINISH));
				// } else {
				// 	sarw->Finish(Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
				// }
				break;
			case RequestStatus::FINISH:
				LOG_MSG("finish")
				isRunning = false;
				// delete this;
				break;
		}
        return false;
    }
	void RecvPackStream::Request::FinishRequest() {
		std::string ret;
		WriteOptions wopts;
		nexus::GenericResponse r;
		r.set_errormessage("success");
		r.set_success(true);
		LOG_ARGS("finish request %s", id.c_str());
		if (repoOpen) {
			auto commitRet = RepoProxy::PackCommit();
			commitRet.wait();
			ret = commitRet.get();
			if (ret != "success") {
				resp = Common::Response(ret.c_str(), false, StatusCode::INTERNAL);
				r.set_errormessage(resp->errorMessage);
				r.set_success(resp->success);
				failureReason = resp->errorMessage;
				requestFailed = true;
				sarw->WriteAndFinish(r, wopts, Status(StatusCode::INTERNAL, resp->errorMessage), reinterpret_cast<void*>(RequestStatus::FINISH));
				RepoProxy::CloseRepo();
				repoOpen = false;
				return;
			}
			RepoProxy::CloseRepo();
			repoOpen = false;

			sarw->WriteAndFinish(r, wopts, Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
			LOG_ARGS("finished request %s", id.c_str())
		}
		// else {
		// 	LOG_ARGS("hmm... %s", id.c_str())
		// 	sarw->WriteAndFinish(r, wopts, Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
		// }
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
	}
	void RecvPackStream::Request::ReadMessage() {
		std::string err;
		grpc::Alarm done;
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
			if (msg.done() == true) {
				LOG_MSG("received done msg")
				done.Set(queue, gpr_time_0(gpr_clock_type::GPR_CLOCK_REALTIME), reinterpret_cast<void*>(RequestStatus::DONE));
				msg.set_data("");
				msg.set_reponame("");
			}
		} else {
			LOG_MSG("not running anymroe")
		}
	}
}}