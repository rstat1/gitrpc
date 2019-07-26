/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/


#ifndef REPOMAN
#define REPOMAN

#include <mutex>
#include <atomic>
#include <future>

#include <base/common.h>
#include <services/git/repository/GitRepo.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

namespace gitrpc { namespace git {
	using namespace nexus::git;
	using namespace base::threading;
	struct OpenRepoArgs {
		const char* repoName;
		std::promise<std::string> result;
	};
	struct WritePackDataArgs {
		size_t size;
		const void* data;
		std::promise<std::string> result;
	};
	struct GenericArgs {
		std::promise<std::string> result;
	};
	struct NewReferenceArgs {
		std::string refName;
		std::string refRev;
		std::promise<std::string> result;
	};
	struct WaitForCommitArgs {
		std::promise<std::future<bool>> result;
	};
	class RepoProxy {
		public:
			static void CloseRepo();
			static std::future<std::string> PackCommit();
			static std::future<std::string> OpenRepo(std::string name);
			static std::future<std::string> PackAppend(const void* data, size_t size);
			static std::future<std::string> CreateReference(std::string refName, std::string refRev);
	};
	class RepositoryManager {
		public:
			void Init();
			void CloseRepo();
			void OpenRepo(void* data);
			void NewReference(void* data);
			void AppendDataToPack(void* data);
			void CommitPackChanges(void* data);
			void WaitForPackCommit(void* data);
		private:
			GitRepo* currentRepo;
			std::mutex repoLocker;
			std::atomic<int> repoRefCnt;
			std::promise<bool> packCommitComplete;

		SINGLETON(RepositoryManager);
	};
}}

#endif