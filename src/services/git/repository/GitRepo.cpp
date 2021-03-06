/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <cassert>

#include <base/logging.h>
#include <services/git/repository/GitRepo.h>
#include <services/git/repository/GitRepoCommon.h>

namespace nexus { namespace git {
	using namespace gitrpc::git;
	using namespace gitrpc::common;
	GitRepo::GitRepo(std::string name) {
		assert(name.size() != 0);
		SetName(name);
		repoPath.append(DEFAULT_REPO_PATH);
		repoPath.append(name);
	}
	GitRepo::~GitRepo() {
		LOG_MSG("cleanup time...")
		wp->free(wp);
		git_odb_free(odb);
		git_repository_free(repo);
	}
	const char* GitRepo::Open(bool bare) {
		int errCode;
		if (bare) {
			errCode = git_repository_open_bare(&repo, repoPath.c_str());
		} else {
			errCode = git_repository_open(&repo, repoPath.c_str());
		}
		if (errCode != 0) { return Common::CheckForError(errCode, "opening repo"); }

		errCode = git_repository_odb(&odb, repo);
		if (errCode != 0) { return Common::CheckForError(errCode, "getting odb ref"); }

		const char* wpInitRet = this->InitWritePackFunctions();
		if (wpInitRet != "success") {
			return wpInitRet;
		}

		return "success";
	}
	const char* GitRepo::InitWritePackFunctions() {
		int errCode = git_odb_write_pack(&wp, odb, GitRepo::TransferProgressCB, nullptr);
		if (errCode != 0) { return Common::CheckForError(errCode, "getting writepack funcptrs"); }
		return "success";
	}
	const char* GitRepo::PackAppend(const void* data, size_t size, git_transfer_progress* stats) {
		int errCode = wp->append(wp, data, size, &tStats);
		if (errCode != 0) { return Common::CheckForError(errCode, "pack append"); }
		return "success";
	}
	const char* GitRepo::PackCommit(git_transfer_progress* stats) {
		int errCode = wp->commit(wp, &tStats);
		if (errCode != 0) { return Common::CheckForError(errCode, "pack commit"); }
		return "success";
	}
	const char* GitRepo::CreateReference(const char* refRev, const char* refName) {
		int errCode;
		git_oid objectID;
		git_reference* newRef;

		LOG_ARGS("add ref %s with hash = %s to repo %s", refName, refRev, GetName().c_str());

		errCode = git_oid_fromstr(&objectID, refRev);
		if (errCode != 0) { return Common::CheckForError(errCode, "failed to convert OID to string"); }
		errCode = git_reference_create(&newRef, repo, refName, &objectID, 0, NULL);
		if (errCode != 0) { return Common::CheckForError(errCode, "failed to create reference"); }
		errCode = git_repository_set_head(repo, refName);
		if (errCode != 0) { return Common::CheckForError(errCode, "failed to set head"); }

		git_reference_free(newRef);
		return "success";
	}
	int GitRepo::TransferProgressCB(const git_transfer_progress* stats, void* payload) {
		// std::cout << stats->received_objects << std::endl;
		return 0;
	}
	GetRefsResponse GitRepo::GetRepoReferences() {
		int errCode;
		git_oid* objectID;
		git_reference* ref;
		char* refID;
		const char* refName;
		git_reference_iterator* refIter;
		std::vector<ReferenceInfo> refs;

		errCode = git_reference_iterator_new(&refIter, repo);
		if (errCode != 0) { return GetRefsResponse(false, Common::CheckForError(errCode, "failed to create reference iterator"), std::vector<gitrpc::git::ReferenceInfo>()); }

		while (git_reference_next(&ref, refIter) != GIT_ITEROVER) {
			refName = git_reference_name(ref);
			errCode = git_reference_name_to_id(objectID, repo, refName);
			if (errCode != 0) { return GetRefsResponse(false, Common::CheckForError(errCode, "failed to get reference ID"), std::vector<gitrpc::git::ReferenceInfo>()); }
			git_oid_tostr(refID, GIT_OID_HEXSZ, objectID);
			refs.push_back(ReferenceInfo(refID, refName));

			git_reference_free(ref);
		}

		git_reference_iterator_free(refIter);

		return GetRefsResponse(true, "success", std::move(refs));
	}
}} // namespace nexus::git