/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/threading/dispatcher/Dispatcher.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

#include <common/GitServiceCommon.h>
#include <services/git/repository/GitRepoCommon.h>
#include <services/git/repository/RepoProxy.h>
#include <services/git/repository/RepositoryManager.h>

namespace gitrpc { namespace git {
	using namespace nexus;
	using namespace base::threading;
	SINGLETON_DEF(RepoProxy)

	std::future<std::string> RepoProxy::OpenRepo(std::string name) {
		OpenRepoArgs* args = new OpenRepoArgs();
		args->repoName = name.c_str();
		NEW_TASK1(OpenRepo, RepositoryManager, RepositoryManager::Get(), OpenRepo, args);
		POST_TASK(OpenRepo, "Main");
		return args->result.get_future();
	}
	void RepoProxy::CloseRepo() {
		NEW_TASK0(CloseRepo, RepositoryManager, RepositoryManager::Get(), CloseRepo);
		POST_TASK(CloseRepo, "Main");
	}
	std::future<std::string> RepoProxy::PackAppend(const void* data, size_t size) {
		WritePackDataArgs* args = new WritePackDataArgs();
		args->data = data;
		args->size = size;
		RANKED_TASK1(AppendToPack, RepositoryManager, RepositoryManager::Get(), AppendDataToPack, TaskPriority::HIGH, args);
		POST_TASK(AppendToPack, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::PackCommit() {
		GenericArgs* args = new GenericArgs();
		RANKED_TASK1(CommitPack, RepositoryManager, RepositoryManager::Get(), CommitPackChanges, TaskPriority::HIGH, args);
		POST_TASK(CommitPack, "Main");
		return args->result.get_future();
	}
	std::future<std::string> RepoProxy::CreateReference(std::string refName, std::string refRev) {
		NewReferenceArgs* args = new NewReferenceArgs();
		args->refName = refName;
		args->refRev = refRev;
		NEW_TASK1(NewRef, RepositoryManager, RepositoryManager::Get(), NewReference, args);
		POST_TASK(NewRef, "Main");
		return args->result.get_future();
	}
	std::future<GetRefsResponse> RepoProxy::GetReferences() {
		GetReferenceListArgs* args = new GetReferenceListArgs();
		NEW_TASK1(GetRefsRequest, RepositoryManager, RepositoryManager::Get(), GetRepoRefs, args);
		POST_TASK(GetRefsRequest, "Main");
		return args->result.get_future();
	}
}} // namespace gitrpc::git