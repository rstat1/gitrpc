/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/logging.h>
#include <services/git/GitRepo.h>

namespace nexus { namespace git {
    using namespace nexus::common;
    GitRepo::GitRepo(std::string name) {
        repoName = name;
    }
    GitRepo::~GitRepo() {
        LOG_MSG("cleanup time...")
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
        return "success";
    }
    git_odb_writepack* GitRepo::WritePack() {
        git_odb_writepack *wp = nullptr;
        int errCode = git_odb_write_pack(&wp, odb, GitRepo::TransferProgressCB, nullptr);
        if (errCode > 0) {
            Common::CheckForError(errCode, "getting writepack funcptrs");
            return nullptr;
        }
        return wp;
    }
    int GitRepo::TransferProgressCB(const git_transfer_progress *stats, void *payload) {
        return 0;
    }
}}