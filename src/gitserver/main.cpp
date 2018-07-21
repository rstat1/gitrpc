#include <BuildInfo.h>
#include <base/common.h>
#include <gitserver/app/AppImpl.h>

using namespace nexus;

int main(int argc, char* argv[]) {
	writeToLog("Init", true);
	LOG_FROM_HERE("Version: %s from the %s branch", BUILDNUMBER, BRANCH)

	AppImpl* app = new AppImpl();
	app->Main();
}