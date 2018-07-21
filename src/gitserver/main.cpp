#include <BuildInfo.h>
#include <base/common.h>

int main(int argc, char* argv[]) {
	writeToLog("Init", true);
	// Log(__FILE__, "Version: %s from the %s branch", BUILDNUMBER, BRANCH);
	LOG_FROM_HERE("Version: %s from the %s branch", BUILDNUMBER, BRANCH)
}