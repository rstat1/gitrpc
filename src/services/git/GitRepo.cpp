/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/logging.h>
#include <services/git/GitRepo.h>

namespace nexus { namespace git {
	GitRepo::GitRepo(std::string name) {
		repoName = name;
	}
	GitRepo::~GitRepo() {
		LOG_MSG("cleanup time...")
		wp->free(wp);
		git_odb_free(odb);
		git_repository_free(repo);
	}
	const char* GitRepo::Open(bool bare) {
		int errCode;
		LOG_ARGS("opening repo %s", repoName.c_str())
		if (bare) {
			errCode = git_repository_open_bare(&repo, repoName.c_str());
		} else {
			errCode = git_repository_open(&repo, repoName.c_str());
		}
		if (errCode > 0) { return Common::CheckForError(errCode, "opening repo"); }

		errCode = git_repository_odb(&odb, repo);
		if (errCode > 0) { return Common::CheckForError(errCode, "getting odb ref"); }

		const char* wpInitRet = this->InitWritePackFunctions();
		if (wpInitRet != "success") {
			return wpInitRet;
		}

		return "success";
	}
	const char* GitRepo::PackAppend(const void *data, size_t size, git_transfer_progress *stats) {
		int errCode = wp->append(wp, data, size, stats);
		if (errCode > 0) { return Common::CheckForError(errCode, "opening repo"); }
		return "success";
	}
	const char* GitRepo::PackCommit(git_transfer_progress *stats) {
		int errCode = wp->commit(wp, stats);
		if (errCode > 0) { return Common::CheckForError(errCode, "opening repo"); }
		return "success";
	}
	const char* GitRepo::InitWritePackFunctions() {
		int errCode = git_odb_write_pack(&wp, odb, GitRepo::TransferProgressCB, nullptr);
		if (errCode > 0) {
			return Common::CheckForError(errCode, "getting writepack funcptrs");
		}
		return "success";
	}
	int GitRepo::TransferProgressCB(const git_transfer_progress *stats, void *payload) {
		return 0;
	}
}}