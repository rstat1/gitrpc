/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <thread>
#include <sys/un.h>
#include <sys/socket.h>

#include <git/git2/refs.h>
#include <grpcpp/server_builder.h>

#include <base/Utils.h>
#include <services/git/GitRepo.h>
#include <services/git/GitServiceImpl.h>

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
	Status GitServiceImpl::ReceivePackStream(ServerContext* context, ServerReader<ReceivePackRequest>* reader, GenericResponse* response) {
		int errCode;
		const char* msg;
		bool repoOpen = false;
		GitRepo* repo = nullptr;
		ReceivePackRequest request;
		git_transfer_progress stats;
		git_odb_writepack* wp = nullptr;
		std::string newRepoPath(DEFAULT_REPO_PATH);
		LOG_ARGS("ReceivePackStream request, repo name = %s", request.reponame().c_str())

		while(reader->Read(&request)) {
			if (repoOpen == false) {
				LOG_ARGS("writing to repo %s...", request.reponame().c_str())
				newRepoPath.append(request.reponame());
				repo = new GitRepo(newRepoPath);
				msg = repo->Open(true);
				if (msg != "success") { return Response(response, msg, false, StatusCode::INTERNAL); }
			 	wp = repo->WritePack();
			 	if (wp == nullptr) { return Response(response, "getting writepack funcptrs", false, StatusCode::INTERNAL); }
				repoOpen = true;
			}
			errCode = wp->append(wp, request.data().data(), request.data().size(), &stats);
			if (errCode > 0) {
				delete repo;
				return Response(response, Common::CheckForError(errCode, "pack append"), false, StatusCode::INTERNAL);
			}
		}
		errCode = wp->commit(wp, &stats);
		if (errCode > 0) {
			delete repo;
			return Response(response, Common::CheckForError(errCode, "pack commit"), false, StatusCode::INTERNAL);
		}
		delete repo;
		return Response(response, "success", true, StatusCode::OK);
	}
	Status GitServiceImpl::UploadPackStream(ServerContext* context, const UploadPackRequest* request, ServerWriter<UploadPackResponse>* writer) {
		return Status(StatusCode::UNIMPLEMENTED, "Work in progress under construction");
	}
	Status GitServiceImpl::InitRepository(ServerContext* context, const InitRepositoryRequest* request, GenericResponse* response) {
		REPO_PATH(request->reponame());
		LOG_ARGS("InitRepository request, repo name = %s", newRepoPath.c_str())

		int errCode;
		const char* ret;
		git_repository* repo;
		git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
		opts.flags = GIT_REPOSITORY_INIT_MKPATH | GIT_REPOSITORY_INIT_BARE | GIT_REPOSITORY_INIT_NO_REINIT;

		errCode = git_repository_init_ext(&repo, newRepoPath.c_str(), &opts);
		if (errCode != 0) { return Response(response, "failed to initialize repo", false, StatusCode::INTERNAL); };

		git_repository_free(repo);
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
		if (errCode != 0) { return Response(response, Common::CheckForError(errCode, "failed to open repo"), false, StatusCode::INTERNAL); };

		errCode = git_oid_fromstr(&objectID, request->refrev().c_str());
		if (errCode != 0) { return Response(response, Common::CheckForError(errCode,"failed to convert OID to string"), false, StatusCode::INTERNAL); };

		errCode = git_reference_create(&newRef, repo, request->refname().c_str(), &objectID, 0, NULL);
		if (errCode != 0) { return Response(response, Common::CheckForError(errCode,"failed to create reference"), false, StatusCode::INTERNAL); };

		errCode = git_repository_set_head(repo, request->refname().c_str());
		if (errCode != 0) { return Response(response, Common::CheckForError(errCode,"failed to set head"), false, StatusCode::INTERNAL); };

		git_repository_free(repo);
		return Response(response, "success", true, StatusCode::OK);
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
	Status GitServiceImpl::Response(GenericResponse* resp, const char* msg, bool success, StatusCode status) {
		if (success) {
			resp->set_errormessage("Success");
			resp->set_success(true);
			return Status::OK;
		} else {
			resp->set_errormessage(msg);
			resp->set_success(false);
			return Status(status, msg);
		}
	}
	int GitServiceImpl::TransferProgressCB(const git_transfer_progress* stats, void* payload) {
		// std::printf("total received objects: %i\n", stats->total_objects);
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