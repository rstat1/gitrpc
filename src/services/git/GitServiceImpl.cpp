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
		int features = git_libgit2_features();
		if ((git_libgit2_features() & GIT_FEATURE_THREADS) != 0) { LOG_MSG("thread support") }
	}
	Status GitServiceImpl::ListKnownRefs(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_MSG("ListKnowRefs request")
		REPO_PATH(request->reponame())

		git_repository* repo;
		git_repository_open_bare(&repo, newRepoPath.c_str());
		git_reference_foreach(repo, GitServiceImpl::GetRepoReferences, response);
		git_repository_free(repo);

		return Status::OK;
	}
	Status GitServiceImpl::ListRefsForClone(ServerContext* context, const ListRefsRequest* request, ListRefsResponse* response) {
		LOG_MSG("ListKnowRefsForClone request");
		REPO_PATH(request->reponame())

		git_oid* hash;
		git_repository* repo;
		git_reference* headRef;
		GitReference* head = new GitReference();

		git_repository_open_bare(&repo, newRepoPath.c_str());
		git_repository_head(&headRef, repo);
		git_reference_foreach(repo, GitServiceImpl::GetRepoReferences, response);
		git_reference_name_to_id(hash, repo, git_reference_name(headRef));

		head->set_referencename("HEAD");
		head->set_referencehash(reinterpret_cast<const char*>(hash->id));

		git_reference_free(headRef);
		git_repository_free(repo);

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
			return Status(StatusCode::INVALID_ARGUMENT, "provided pack file had a length equal to 0");
		}

		LOG_ARGS("data length %i", request->data().size())

		CHECK(git_repository_open_bare(&repo, newRepoPath.c_str()), "Failed to open repo", SUCCESS([&]() {
			int errCode = git_repository_odb(&odb, repo);
			if (errCode > 0) {
				return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "opening odb"));
			};
			CHECK(git_odb_write_pack(&wp, odb, GitServiceImpl::TransferProgressCB, nullptr),
				"failed to get writepack funcptrs", SUCCESS([&]{
					CHECK(wp->append(wp, request->data().data(), request->data().size(), &stats), "Failed writing pack", SUCCESS([&]() {
						LOG_MSG("wrote pack successfully");
						CHECK(wp->commit(wp, &stats), "failed commiting pack", SUCCESS([&](){}))
						git_odb_free(odb);
						git_repository_free(repo);
					}));
			}));
			return Status::OK;
		}));
		response->set_success(true);
		return Status::OK;
	}
	Status GitServiceImpl::ReceivePackStream(ServerContext* context, ServerReader<ReceivePackRequest>* reader, GenericResponse* response) {
		git_libgit2_init();

		int errCode;
		git_odb *odb;
		grpc::Status ret;
		ReceivePackRequest request;
		git_transfer_progress stats;
		git_odb_writepack* wp = nullptr;
		git_repository* repo;
		std::string newRepoPath(DEFAULT_REPO_PATH);
		newRepoPath.append("new-server");
		LOG_ARGS("ReceivePackStream request, repo name = %s", request.reponame().c_str())

		errCode = git_repository_open_bare(&repo, newRepoPath.c_str());
		if (errCode > 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "opening repo")); };
		errCode = git_repository_odb(&odb, repo);
		if (errCode > 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "opening odb")); };
		errCode = git_odb_write_pack(&wp, odb, GitServiceImpl::TransferProgressCB, nullptr);
		if (errCode > 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "get writepack funcptrs")); };

		while(reader->Read(&request)) {
			LOG_MSG("writing...")

			errCode = wp->append(wp, request.data().data(), request.data().size(), &stats);
			if (errCode > 0) {
				response->set_success(false);
				return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "appending to packfile"));
			};
			errCode = wp->commit(wp, &stats);
			if (errCode > 0) {
				response->set_success(false);
				return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "commiting packfile changes"));
			};
			// ret = this->WriteToPack(request.data().data(), request.data().size(), repo, wp, true) ?
			// 	Status::OK : Status(StatusCode::INTERNAL, "WriteToPack: write failed");
		}
		response->set_success(true);
		response->set_errormessage("success");
		return Status::OK;
	}
	Status GitServiceImpl::UploadPackStream(ServerContext* context, const UploadPackRequest* request, ServerWriter<UploadPackResponse>* writer) {
		return Status(StatusCode::UNIMPLEMENTED, "Work in progress under construction");
	}
	bool GitServiceImpl::WriteToPack(const void* data, size_t size, git_repository* repo, git_odb_writepack *wp, bool commit) {
		const char* ret;
		int errCode = 0;
		git_transfer_progress stats;
		if (size > 0) {
			errCode = wp->append(wp, data, size, &stats);
			if (errCode > 0) {
				this->CheckForError(errCode, "pack append");
				return false;
			}
		}

		if (commit) {
			errCode = wp->commit(wp, &stats);
			if (errCode > 0) {
				this->CheckForError(errCode, "pack commit");
				return false;
			}
		}

		return true;
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
		git_oid objectID;
		git_repository* repo;
		git_reference* newRef;
		std::string newRepoPath(DEFAULT_REPO_PATH);
		newRepoPath.append("new-server");

		LOG_ARGS("add ref %s, hash = %s", request->refname().c_str(), request->refrev().c_str());

		int errCode = git_repository_open_bare(&repo, newRepoPath.c_str());
		if (errCode != 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "opening repo")); };

		errCode = git_oid_fromstr(&objectID, request->refrev().c_str());
		if (errCode != 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "git_oid_fromstr")); };

		errCode = git_reference_create(&newRef, repo, request->refname().c_str(), &objectID, 0, NULL);
		if (errCode != 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "git_reference_create")); };

		errCode = git_repository_set_head(repo, request->refname().c_str());
		if (errCode != 0) { return Status(StatusCode::INTERNAL, this->CheckForError(errCode, "git_repository_set_head")); };

		git_repository_free(repo);
		response->set_success(true);
		response->set_errormessage("success");
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
		return newRepoPath;
	}
	int GitServiceImpl::GetRepoReferences(git_reference* ref, void* payload) {
		ListRefsResponse* resp = (ListRefsResponse*)payload;
		const char* refName = git_reference_name(ref);
		GitReference* refInfo = resp->add_refs();
		refInfo->set_referencename(refName);
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
		std::printf("total received objects: %i\n", stats->total_objects);
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