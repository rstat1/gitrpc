/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <grpcpp/alarm.h>
#include <services/git/requests/RecvPackStream.h>

namespace nexus { namespace git {
    void RecvPackStream::StartHandlerThread() {
		requestHandler.reset(new std::thread(std::bind(&RecvPackStream::HandlerThread, this)));
	}
    void RecvPackStream::HandlerThread() {
		bool ok = false;
		grpc::Alarm alarm;
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
			if (last == RequestStatus::FINISH) {
				LOG_MSG("hello")
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
				sarw->Finish(Status::OK, reinterpret_cast<void*>(RequestStatus::FINISH));
				break;
			case RequestStatus::FINISH:
				LOG_MSG("finish")
				delete this;
				break;
		}
        return false;
    }
	void RecvPackStream::Request::WriteResponse() {
		nexus::GenericResponse r;
		r.set_errormessage("Success");
		r.set_success(true);
		sarw->Write(r, reinterpret_cast<void*>(RequestStatus::WRITE));
		LOG_MSG("write")
	}
	void RecvPackStream::Request::ReadMessage() {
		if (isRunning) {
			sarw->Read(&msg, reinterpret_cast<void*>(RequestStatus::READ));
			LOG_ARGS("read %i", msg.data().size())
		}
	}
}}
        // grpc::Alarm alarm;
		// if (status == RequestStatus::CONNECT) {
		// 	// LOG_ARGS("%p connect", this);
		// 	status = RequestStatus::READ;
		// } else if (status == RequestStatus::READ) {
		// 	// LOG_ARGS("%p read", this);
		// 	reader->Read(&msg, this);
		// 	status = RequestStatus::DONE;
		// 	return false;
		// 	// new ReceivePack(svc, queue);//gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME)

		// } else if (status == RequestStatus::DONE) {
		// 	// LOG_ARGS("%p done", this)
		// 	if (!notifiedDone) {
		// 		// LOG_ARGS("%p notifiedDone = false", this)
		// 		notifiedDone = true;
		// 		status = RequestStatus::CONNECT;
		// 		Cleanup();
		// 	} else {
		// 		// LOG_ARGS("%p done yet.", this)
		// 	}
		// }
		// if (status == RequestStatus::DONE) { alarm.Set(queue, gpr_time_0(gpr_clock_type::GPR_CLOCK_REALTIME), this); }
		// return true;