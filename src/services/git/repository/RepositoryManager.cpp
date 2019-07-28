/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/
#include <base/threading/dispatcher/DispatcherTypes.h>
#include <services/git/repository/RepositoryManager.h>

namespace gitrpc { namespace git {
	SINGLETON_DEF(RepositoryManager)
	void RepositoryManager::Init() {
		git_libgit2_init();
		repoRefCnt.store(0);
	}
	void RepositoryManager::OpenRepo(void* data) {
		OpenRepoArgs* repoArgs = (OpenRepoArgs*)data;
		if (currentRepo == nullptr || (currentRepo != nullptr && repoArgs->repoName != currentRepo->GetName())) {
			currentRepo = new GitRepo(repoArgs->repoName);
			repoArgs->result.set_value(currentRepo->Open(true));
			repoRefCnt.store(0);
		} else {
			repoRefCnt++;
			repoArgs->result.set_value("success");
		}
	}
	void RepositoryManager::AppendDataToPack(void* data) {
		std::lock_guard locker(repoLocker);
		WritePackDataArgs* args = (WritePackDataArgs*)data;
		args->result.set_value(currentRepo->PackAppend(args->data, args->size, nullptr));
	}
	void RepositoryManager::CommitPackChanges(void* data) {
		std::lock_guard locker(repoLocker);
		GenericArgs* args = (GenericArgs*)data;
		std::string ret(currentRepo->PackCommit(nullptr));
		if (currentRepo == nullptr) {
			args->result.set_value("repo not open");
		} else {
			args->result.set_value(ret);
		}
	}
	void RepositoryManager::WaitForPackCommit(void* data) {
		WaitForCommitArgs* args = (WaitForCommitArgs*)data;
		args->result.set_value(packCommitComplete.get_future());
		//TODO: Return a promise that waits for CommitPackChanges to be called.
	}
	void RepositoryManager::NewReference(void* data) {
		std::lock_guard locker(repoLocker);
		NewReferenceArgs* args = (NewReferenceArgs*)data;
		args->result.set_value(currentRepo->CreateReference(args->refRev.c_str(), args->refName.c_str()));
	}
	void RepositoryManager::CloseRepo() {
		std::lock_guard locker(repoLocker);
		if (repoRefCnt == 0) {
			delete currentRepo;
			currentRepo = nullptr;
		} else {
			repoRefCnt--;
		}

	}
	void RepoProxy::CloseRepo() {
		NEW_TASK0(CloseRepo, RepositoryManager, RepositoryManager::Get(), CloseRepo);
		POST_TASK(CloseRepo, "Main");
	}
	std::future<std::string> RepoProxy::OpenRepo(std::string name) {
		OpenRepoArgs* args = new OpenRepoArgs();
		args->repoName = name.c_str();
		NEW_TASK1(OpenRepo, RepositoryManager, RepositoryManager::Get(), OpenRepo, args);
		POST_TASK(OpenRepo, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::PackCommit() {
		GenericArgs* args = new GenericArgs();
		NEW_TASK1(OpenRepo, RepositoryManager, RepositoryManager::Get(), CommitPackChanges, args);
		POST_TASK(OpenRepo, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::PackAppend(const void* data, size_t size) {
		WritePackDataArgs* args = new WritePackDataArgs();
		args->data = data;
		args->size = size;
		NEW_TASK1(OpenRepo, RepositoryManager, RepositoryManager::Get(), AppendDataToPack, args);
		POST_TASK(OpenRepo, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::CreateReference(std::string refName, std::string refRev) {
		NewReferenceArgs* args = new NewReferenceArgs();
		args->refName = refName;
		args->refRev = refRev;
		NEW_TASK1(OpenRepo, RepositoryManager, RepositoryManager::Get(), NewReference, args);
		POST_TASK(OpenRepo, "Main");
		return args->result.get_future();
	}
}}