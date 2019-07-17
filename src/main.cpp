#include <BuildInfo.h>
#include <base/common.h>
#include <app/AppImpl.h>

using namespace nexus;

int main(int argc, char* argv[]) {
	InitLog("gitrpc.log");
	LOG_ARGS("GitRPC Version: %s.%s", BUILDNUMBER, BRANCH)

	AppImpl* app = new AppImpl();
	app->Main();
}