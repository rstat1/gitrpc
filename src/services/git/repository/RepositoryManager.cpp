/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <services/git/repository/GitRepoCommon.h>
#include <services/git/repository/RepositoryManager.h>

namespace gitrpc { namespace git {
	SINGLETON_DEF(RepositoryManager)

	void RepositoryManager::Init() {
		git_libgit2_init();
		repoRefCnt.store(0);
	}
	void RepositoryManager::OpenRepo(void* data) {
		const char* result;
		std::lock_guard guard(repoLocker);
		OpenRepoArgs* repoArgs = (OpenRepoArgs*)data;
		LOG_ARGS("open repo %s", repoArgs->repoName)
		if (currentRepo == nullptr || (currentRepo != nullptr && repoArgs->repoName != currentRepo->GetName())) {
			currentRepo = new GitRepo(repoArgs->repoName);
			result = currentRepo->Open(true);
			if (strcmp(result, "success") == 0) {
				repoRefCnt.store(0);
			}
			repoArgs->result.set_value(result);
		} else {
			LOG_MSG("increment repo ref count")
			repoArgs->result.set_value("success");
			repoRefCnt++;
		}
	}
	void RepositoryManager::CloseRepo() {
		std::lock_guard guard(repoLocker);
		delete currentRepo;
		currentRepo = nullptr;
		repoRefCnt.store(0);
	}
	void RepositoryManager::AppendDataToPack(void* data) {
		WritePackDataArgs* args = (WritePackDataArgs*)data;
		std::lock_guard guard(repoLocker);
		args->result.set_value(currentRepo->PackAppend(args->data, args->size, nullptr));
	}
	void RepositoryManager::CommitPackChanges(void* data) {
		LOG_MSG("Commit pack to disk")
		GenericArgs* args = (GenericArgs*)data;
		std::lock_guard guard(repoLocker);
		if (currentRepo == nullptr) {
			args->result.set_value("repo not open");
		} else {
			std::string ret(currentRepo->PackCommit(nullptr));
			args->result.set_value(ret);
		}
	}
	void RepositoryManager::NewReference(void* data) {
		LOG_MSG("Write new reference")
		NewReferenceArgs* args = (NewReferenceArgs*)data;
		std::lock_guard guard(repoLocker);
		if (currentRepo != nullptr) {
			args->result.set_value(currentRepo->CreateReference(args->refRev.c_str(), args->refName.c_str()));
		} else {
			LOG_MSG("hmm...")
			args->result.set_value("nope");
		}
	}
	void RepositoryManager::GetRepoRefs(void* data) {
		std::lock_guard guard(repoLocker);
		GetReferenceListArgs* args = (GetReferenceListArgs*)data;

		if (currentRepo != nullptr) {
			args->result.set_value(currentRepo->GetRepoReferences());
		} else {
			args->result.set_value(GetRefsResponse(false, "no repo open", std::vector<ReferenceInfo>()));
		}

	}
}} // namespace gitrpc::git