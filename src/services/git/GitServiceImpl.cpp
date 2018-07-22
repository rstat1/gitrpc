/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>
#include <sys/un.h>
#include <sys/socket.h>
#include <base/Utils.h>
#include <grpcpp/server_builder.h>
#include <services/git/GitServiceImpl.h>
#include <git/git2.h>

namespace nexus { namespace git {
	using namespace base::utils;
	using namespace nexus::common;
	SINGLETON_DEF(GitServiceImpl);

	void GitServiceImpl::InitGitService() {
		git_libgit2_init();
	}
	Status GitServiceImpl::ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_FROM_HERE_E("ListKnowRefs request")
		return Status::OK;
	}
	Status GitServiceImpl::ListRefsForClone(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_FROM_HERE_E("ListKnowRefsForClone request")
		return Status::OK;
	}
	Status GitServiceImpl::UploadPack(ServerContext* context, const UploadPackRequest* request, UploadPackResponse* response) {
		LOG_FROM_HERE_E("UploadPack request")
		return Status::OK;
	}
	Status GitServiceImpl::ReceivePack(ServerContext *context, const ReceivePackRequest *request, GenericResponse* response) {
		LOG_FROM_HERE_E("ReceivePack request")
		return Status::OK;
	}
	Status GitServiceImpl::InitRepository(ServerContext *context, const InitRepositoryRequest *request, GenericResponse* response) {
		LOG_FROM_HERE_E("InitRepository request");

		const char* ret;
		git_repository* repo;
		git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
		const char* newRepoName = request->reponame().c_str();
		std::string newRepoPath("/home/rstat1/Apps/nexus-repos/");

		newRepoPath.append(newRepoName);
		opts.flags |= GIT_REPOSITORY_INIT_MKPATH;

		ret = this->CheckForError(git_repository_init_ext(&repo, newRepoPath.c_str() ,&opts),
			"Failed to init repo");

		if (strcmp(ret, "") == 0) {
			response->set_errormessage("Success");
			response->set_success(true);
		} else {
			response->set_errormessage(ret);
			response->set_success(false);
		}
		return Status::OK;
	}
	const char* GitServiceImpl::CheckForError(int errCode, const char *message) {
		const git_error* err;

		if (errCode == 0) {
			return "";
		} else {
			err = giterr_last();
			if (err != nullptr && err->message != NULL) {
				LOG_FROM_HERE("%s: %s", message, err->message);
				return err->message;
			}
		}
		return "";
	}
}}