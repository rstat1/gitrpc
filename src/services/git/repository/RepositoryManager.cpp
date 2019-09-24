/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/
#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
#include <services/git/repository/RepositoryManager.h>

namespace gitrpc { namespace git {
	SINGLETON_DEF(RepoProxy)
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
	void RepositoryManager::CloseRepo() {
		std::lock_guard guard(repoLocker);
		delete currentRepo;
		currentRepo = nullptr;
		repoRefCnt.store(0);
	}
	void RepoProxy::CloseRepo() {
		NEW_TASK0(CloseRepo, RepositoryManager, RepositoryManager::Get(), CloseRepo);
		POST_TASK(CloseRepo, "Main");
	}
	std::future<std::string> RepoProxy::OpenRepo(std::string name) {
		OpenRepoArgs* args = new OpenRepoArgs();
		// std::promise<std::string>().swap(args->result);
		args->repoName = name.c_str();
		NEW_TASK1(OpenRepo, RepositoryManager, RepositoryManager::Get(), OpenRepo, args);
		POST_TASK(OpenRepo, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::PackCommit() {
		GenericArgs* args = new GenericArgs();
		// std::promise<std::string>().swap(args->result);
		RANKED_TASK1(CommitPack, RepositoryManager, RepositoryManager::Get(), CommitPackChanges, TaskPriority::HIGH, args);
		POST_TASK(CommitPack, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::PackAppend(const void* data, size_t size) {
		WritePackDataArgs* args = new WritePackDataArgs();
		// std::promise<std::string>().swap(args->result);
		args->data = data;
		args->size = size;
		RANKED_TASK1(AppendToPack, RepositoryManager, RepositoryManager::Get(), AppendDataToPack, TaskPriority::HIGH, args);
		POST_TASK(AppendToPack, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::CreateReference(std::string refName, std::string refRev) {
		NewReferenceArgs* args = new NewReferenceArgs();
		// std::promise<std::string>().swap(args->result);
		args->refName = refName;
		args->refRev = refRev;
		LOG_MSG("create ref request")
		NEW_TASK1(NewRef, RepositoryManager, RepositoryManager::Get(), NewReference, args);
		POST_TASK(NewRef, "Main");
		return args->result.get_future();
	}
}} // namespace gitrpc::git