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

#include <git/git2/refs.h>

#define DEFAULT_REPO_PATH "/home/rstat1/Apps/test/"

namespace nexus { namespace git {
	using namespace base::utils;
	using namespace nexus::common;
	using namespace std::placeholders;
	SINGLETON_DEF(GitServiceImpl);

	void GitServiceImpl::InitGitService() {
		LOG_MSG("init libgit");
		git_libgit2_init();
	}
	Status GitServiceImpl::ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_MSG("ListKnowRefs request")
		REPO_PATH(request->reponame())

		git_repository* repo;
		git_repository_open(&repo, newRepoPath.c_str());
		git_reference_foreach(repo, GitServiceImpl::GetRepoReferences, response);
		git_repository_free(repo);

		return Status::OK;
	}
	Status GitServiceImpl::ListRefsForClone(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_MSG("ListKnowRefsForClone request");
		return Status::OK;
	}
	Status GitServiceImpl::UploadPack(ServerContext* context, const UploadPackRequest* request, UploadPackResponse* response) {
		LOG_MSG("UploadPack request")

		return Status::OK;
	}
	Status GitServiceImpl::ReceivePack(ServerContext* context, const ReceivePackRequest* request, GenericResponse* response) {
		LOG_ARGS("ReceivePack request, repo name = %s", request->reponame().c_str())
		REPO_PATH("new-server")

		git_odb *odb;
		const char* ret;
		git_repository* repo;
		git_transfer_progress stats;
		git_odb_writepack *wp = NULL;
		if (!request->data().length()) {
			LOG_MSG("pack is empty")
			this->FillInGenericResponse(response, "empty pack", false);
			return Status::OK;
		}

		LOG_ARGS("data length %i", request->data().size())

		CHECK(git_repository_open_bare(&repo, newRepoPath.c_str()), "Failed to open repo", SUCCESS([&]() {
			if (git_repository_odb(&odb, repo) > 0) {
				return Status::CANCELLED;
			};
			CHECK(git_odb_write_pack(&wp, odb, GitServiceImpl::TransferProgressCB, nullptr),
				"failed to get writepack funcptrs", SUCCESS([&]{
					CHECK(wp->append(wp, request->data().data(), request->data().size(), &stats), "Failed writing pack", SUCCESS([&]() {
						LOG_MSG("wrote pack successfully");
						CHECK(wp->commit(wp, &stats), "failed commiting pack", SUCCESS([&](){}))
						this->WalkAndPrintObjectIDs(repo);
						git_repository_free(repo);
					}));
			}));
			return Status::OK;
		}));
		response->set_success(true);
		return Status::OK;
	}
	Status GitServiceImpl::InitRepository(ServerContext* context, const InitRepositoryRequest* request, GenericResponse* response) {
		REPO_PATH(request->reponame());
		LOG_ARGS("InitRepository request, repo name = %s", newRepoPath.c_str())

		const char* ret;
		git_repository* repo;
		git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
		opts.flags = GIT_REPOSITORY_INIT_MKPATH | GIT_REPOSITORY_INIT_BARE | GIT_REPOSITORY_INIT_NO_REINIT;

		CHECK(git_repository_init_ext(&repo, newRepoPath.c_str(), &opts), "Failed to init repo", SUCCESS([&](){
			git_repository_free(repo);
		}));

		return Status::OK;
	}
	Status GitServiceImpl::WriteReference(ServerContext* context, const WriteReferenceRequest* request, GenericResponse* response) {
		REPO_PATH("new-server");
		git_repository* repo;

		LOG_ARGS("add ref %s, hash = %s", request->refname().c_str(), request->refrev().c_str());

		CHECK(git_repository_open_bare(&repo, newRepoPath.c_str()), "Failed to open repo", SUCCESS([&]() {
			git_oid* objectID;
			git_reference* newRef;

			git_oid_fromstr(objectID, request->refrev().c_str());
			CHECK(git_reference_create(&newRef, repo, request->refname().c_str(), objectID, 0, NULL), "failed to create reference", SUCCESS([&]() {
					CHECK(git_repository_set_head(repo, request->refname().c_str()), "failed setting head to new ref", SUCCESS([&](){ LOG_MSG("ref set successfully!"); }));
					git_repository_free(repo);
				})
			);
		}));

		return Status::OK;
	}
	const char* GitServiceImpl::CheckForError(int errCode, const char* message) {
		const git_error* err;
		if (errCode == 0) {	return ""; }
		else {
			err = giterr_last();
			if (err != nullptr && err->message != NULL) {
				LOG_ARGS("%s: %s", message, err->message);
				return err->message;
			}
		}
		return "";
	}
	std::string GitServiceImpl::ConvertRepoNameToPath(std::string name) {
		std::string newRepoPath(DEFAULT_REPO_PATH);
		newRepoPath.append(name);
		newRepoPath.append("/");
		// LOG_ARGS("repo path = %s", newRepoPath.c_str())

		return newRepoPath;
	}
	int GitServiceImpl::GetRepoReferences(git_reference* ref, void* payload) {
		ListRefsResponse* resp = (ListRefsResponse*)payload;

		GitReference* refInfo = resp->add_refs();
		refInfo->set_referencename(git_reference_name(ref));
		// refInfo->set_referencehash(git_reference_)

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
		LOG_ARGS("total received objects: %i", stats->total_objects);
		return 0;
	}
	void GitServiceImpl::WalkAndPrintObjectIDs(git_repository* repo) {
		git_oid *oid;
		git_revwalk* out;
		git_commit *commit = nullptr;
		if (!git_revwalk_new(&out, repo)) {
			// while (git_revwalk_next(oid, out) == 0) {
			// 	LOG_ARGS("object id: %s", git_oid_tostr_s(oid));
			// }
			// if (strcmp(this->CheckForError(git_revwalk_next(oid, out), "failed to revwalk_next"), "") == 0) {
			// } else {
			// 	LOG_MSG("failed to revwalk_next");
			// }
			// for (; !git_revwalk_next(oid, out); git_commit_free(commit)) {
			// }
		} else {
			LOG_MSG("failed to revwalk");
		}
	}
}}