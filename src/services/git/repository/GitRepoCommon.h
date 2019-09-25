/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef RPCOMMON
#define RPCOMMON

#include <future>
#include <string>
#include <vector>

namespace gitrpc { namespace git {
	struct ReferenceInfo {
		std::string refName;
		std::string refHash;
		ReferenceInfo(std::string name, std::string hash) : refName(name), refHash(hash) {}
	};
	struct GetRefsResponse {
		bool success;
		const char* error;
		std::vector<gitrpc::git::ReferenceInfo> refs;
		GetRefsResponse(bool success, const char* result, std::vector<gitrpc::git::ReferenceInfo> refList) : success(success), error(result), refs(refList) {}
	};
	struct OpenRepoArgs {
		const char* repoName;
		std::promise<std::string> result;
	};
	struct WritePackDataArgs {
		size_t size;
		const void* data;
		std::string repoName;
		std::promise<std::string> result;
	};
	struct GenericArgs {
		std::promise<std::string> result;
	};
	struct NewReferenceArgs {
		std::string refName;
		std::string refRev;
		std::string repoName;
		std::promise<std::string> result;
	};
	struct GetReferenceListArgs {
		std::string repoName;
		std::promise<GetRefsResponse> result;
	};
}} // namespace gitrpc::git

#endif