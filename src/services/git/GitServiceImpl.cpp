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

#define DEFAULT_REPO_PATH "/home/rstat1/Apps/nexus-repos/"

namespace nexus { namespace git {
	using namespace base::utils;
	using namespace nexus::common;
	using namespace std::placeholders;
	SINGLETON_DEF(GitServiceImpl);

	void GitServiceImpl::InitGitService() {
		git_libgit2_init();

	}
	Status GitServiceImpl::ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_FROM_HERE_E("ListKnowRefs request")
		REPO_PATH(request->reponame())

		git_repository* repo;
		git_repository_open(&repo, newRepoPath.c_str());
		git_reference_foreach(repo, GitServiceImpl::GetRepoReferences, response);
		git_repository_free(repo);

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
	Status GitServiceImpl::ReceivePack(ServerContext* context, const ReceivePackRequest* request, GenericResponse* response) {
		LOG_FROM_HERE("ReceivePack request, repo name = %s", request->reponame().c_str())
		REPO_PATH(request->reponame())

		git_odb *odb;
		const char* ret;
		git_repository* repo;
		git_transfer_progress stats;
		git_odb_writepack *wp = NULL;
		if (!request->data().length()) {
			LOG_FROM_HERE_E("pack is empty")
			this->FillInGenericResponse(response, "empty pack", false);
			return Status::OK;
		}

		LOG_FROM_HERE("data length %i", request->data().length())

		CFE(git_repository_open_bare(&repo, newRepoPath.c_str()), "Failed to open repo", VF([&]() {
			git_repository_odb(&odb, repo);
			CFE(git_odb_write_pack(&wp, odb, GitServiceImpl::TransferProgressCB, nullptr),
				"failed to get writepack funcptrs", VF([&]{
					CFE(wp->append(wp, request->data().data(), request->data().size(), &stats),
						"Failed writing pack",
						VF([&]() {
							LOG_FROM_HERE_E("wrote pack successfully");
							CFE(wp->commit(wp, &stats), "failed commiting pack", VF([&](){}))
							git_repository_free(repo);
						})
					);
			}));
		}));

		return Status::OK;
	}
	Status GitServiceImpl::InitRepository(ServerContext* context, const InitRepositoryRequest* request, GenericResponse* response) {
		REPO_PATH(request->reponame());
		LOG_FROM_HERE("InitRepository request, repo name = %s", newRepoPath.c_str())

		const char* ret;
		git_repository* repo;
		git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
		opts.flags = GIT_REPOSITORY_INIT_MKPATH | GIT_REPOSITORY_INIT_BARE | GIT_REPOSITORY_INIT_NO_REINIT;

		CFE(git_repository_init_ext(&repo, newRepoPath.c_str(), &opts), "Failed to init repo", VF([&](){
			git_repository_free(repo);
		}));

		return Status::OK;
	}
	Status GitServiceImpl::WriteReference(ServerContext* context, const WriteReferenceRequest* request, GenericResponse* response) {
		return Status::OK;
	}
	const char* GitServiceImpl::CheckForError(int errCode, const char* message) {
		const git_error* err;
		if (errCode == 0) {	return ""; }
		else {
			err = giterr_last();
			if (err != nullptr && err->message != NULL) {
				LOG_FROM_HERE("%s: %s", message, err->message);
				return err->message;
			}
		}
		return "";
	}
	std::string GitServiceImpl::ConvertRepoNameToPath(std::string name) {
		std::string newRepoPath(DEFAULT_REPO_PATH);
		newRepoPath.append(name);
		newRepoPath.append("/");
		// LOG_FROM_HERE("repo path = %s", newRepoPath.c_str())

		return newRepoPath;
	}
	int GitServiceImpl::GetRepoReferences(git_reference* ref, void* payload) {
		return 0;
	}
	void GitServiceImpl::FillInGenericResponse(GenericResponse* resp, const char* msg, bool success) {
		if (strcmp("ret", "") == 0) {
			resp->set_errormessage("Success");
			resp->set_success(true);
		} else {
			resp->set_errormessage(msg);
			resp->set_success(false);
		}
	}
	int GitServiceImpl::TransferProgressCB(const git_transfer_progress* stats, void* payload) {
		LOG_FROM_HERE("total received objects: %i", stats->total_objects);
		return 0;
	}
}}