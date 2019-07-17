/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GSIMPL
#define GSIMPL

#include <git/git2.h>
#include <base/common.h>
#include <grpcpp/server.h>
#include <common/GitServiceCommon.h>

#include <grpcpp/support/server_interceptor.h>

namespace nexus { namespace git {
	using namespace grpc;
	using namespace nexus::common;
	using namespace google::protobuf;

	class GitServiceImpl : public GitService::Service {
		public:
			void InitGitService();
			Status ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response);
			Status ListRefsForClone(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response);
			Status UploadPack(ServerContext* context, const UploadPackRequest* request, UploadPackResponse* response);
			Status ReceivePack(ServerContext* context, const ReceivePackRequest* request, GenericResponse* response);
			Status InitRepository(ServerContext* context, const InitRepositoryRequest* request, GenericResponse* response);
			Status WriteReference(ServerContext* context, const WriteReferenceRequest* request, GenericResponse* response);
			Status ReceivePackStream(ServerContext* context, ServerReader<ReceivePackRequest>* reader, GenericResponse* response);
			Status UploadPackStream(ServerContext* context, const UploadPackRequest* request, ServerWriter<UploadPackResponse>* writer);

		private:
			void WalkAndPrintObjectIDs(git_repository* repo);
			void FillInGenericResponse(GenericResponse* resp, const char* msg, bool success);
			std::string ConvertRepoNameToPath(std::string name);
			const char* CheckForError(int errCode, const char* message);
			static int GetRepoReferences(git_reference* ref, void* payload);
			static int TransferProgressCB(const git_transfer_progress *stats, void *payload);
			bool WriteToPack(const void* data, size_t size, git_repository* repo, git_odb_writepack *wp, bool commit);

			std::unique_ptr<Server> server;

		SINGLETON(GitServiceImpl);
	};
	class GitServiceInterceptor : public grpc::experimental::Interceptor {
		public:
			GitServiceInterceptor(grpc::experimental::ServerRpcInfo* info) : info(info) { LOG_MSG("created interceptor"); }
			void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override {
				LOG_ARGS("method: %s", info->method());
				methods->Proceed();
			}
		private:
			grpc::experimental::ServerRpcInfo* info;
	};
	class GitServiceInterceptorFactory : public grpc::experimental::ServerInterceptorFactoryInterface {
		public:
			grpc::experimental::Interceptor* CreateServerInterceptor(grpc::experimental::ServerRpcInfo* info) override {
				return new GitServiceInterceptor(info);
			}
	};
}}

#endif