#!/usr/bin/python
import os
import sys
import glob
import json
import shlex
import platform
import subprocess
import urllib, zipfile

from datetime import datetime, date
from shutil import make_archive, rmtree
from distutils.dir_util import copy_tree

global arch, ClientID, ClientKey, URLPrefix, WSURLPrefix

path = os.path.abspath(os.path.split(__file__)[0])
currentOS = ""
outputDir = ""

#TODO: Switch from dev to prod with a cmdline switch?
#TODO: Load the following values from a file to maybe make all this more generic.
rootNamespace = "nexus"
ClientID = ""
ClientKey = ""
URLPrefix = ""
WSURLPrefix = ""

if (os.path.isdir(os.getcwd() + "/external") == False):
        print "Please execute this script from the root of your source tree."
        exit(1)

def InitBuildEnv():
        global buildDir, arch
        GNCmd = "build/gn gen "
        buildDir = os.getcwd()
        if os.environ.has_key("TARGET_ARCH"):
                arch = os.environ["TARGET_ARCH"]
        else:
                arch = "x86_64"
                WriteX64GNFlags()
        if platform.system() == "Linux":
                if (arch == "armv7l"):
                        WriteARMGNFlags()
                currentOS = "linux"
        elif platform.system() == "Windows":
                currentOS = "win"
        GNCmd += "out-" + arch
        subprocess.check_call(GNCmd, shell=True)
        if os.path.exists('build/service-details.json'):
                with open('build/service-details.json', 'r') as details:
                        info = json.loads(details)
                        ClientID = info["ClientID"]
                        ClientKey = info["ClientKey"]
                        URLPrefix = info["URLPrefix"]
                        WSURLPrefix = info["WSURLPrefix"]
                        rootNamespace = info["RootNamespace"]


def WriteARMGNFlags():
        if not os.path.exists('out-armv7l'):
                os.makedirs('out-armv7l')
        if os.environ.has_key("STEAMLINK_SDK_PATH"):
                SetSteamLinkEnvVars();
        else:
                with open('out-armv7l/args.gn', 'w+') as argsFile:
                        argsFile.write('target_cpu = "arm"\n')

def SetSteamLinkEnvVars():
        currPath = os.environ["PATH"]
        sdkBinPath = os.environ["STEAMLINK_SDK_PATH"] + "/bin"
        tcPath = os.environ["STEAMLINK_SDK_PATH"] + "/toolchain/bin"

        os.environ["CROSS"] = "armv7a-cros-linux-gnueabi-"
        os.environ["PATH"] = sdkBinPath + ":" + tcPath + ":" + currPath
        os.environ["MARVELL_ROOTFS"] = os.environ["STEAMLINK_SDK_PATH"] + "rootfs"
        os.environ["CC"] = os.environ["CROSS"] + "gcc --sysroot=" + os.environ["MARVELL_ROOTFS"]
        os.environ["CXX"] = os.environ["CROSS"] + "g++ --sysroot=" + os.environ["MARVELL_ROOTFS"]
        with open('out-armv7l/args.gn', 'w+') as argsFile:
                      argsFile.write('target_cpu = "arm"\n')
                      argsFile.write('target_os = "steamlink"\n')
                      argsFile.write('use_sysroot = true\n')
                      argsFile.write('arm_float_abi = "hard"\n')
                      argsFile.write('target_sysroot = "' + os.environ["MARVELL_ROOTFS"] + '"')

def WriteX64GNFlags():
        if not os.path.exists('out-x86_64'):
                os.makedirs('out-x86_64')
        with open('out-x86_64/args.gn', 'w+') as argsFile:
                argsFile.write('is_clang = true')

def GetServiceDetails():
        global ClientID, ClientKey, URLPrefix, WSURLPrefix
        if os.environ.has_key("CLIENT_ID"):
                ClientID = os.environ["CLIENT_ID"]
        if os.environ.has_key("CLIENT_KEY"):
                ClientKey = os.environ["CLIENT_KEY"]
        if os.environ.has_key("URL_PREFIX"):
                URLPrefix = os.environ["URL_PREFIX"]
        if os.environ.has_key("WS_PREFIX"):
                WSURLPrefix = os.environ["WS_PREFIX"]
        if os.environ.has_key("ROOT_NS"):
                rootNamespace = os.environ["ROOT_NS"]

def GetBranchName():
        if os.environ.has_key("CI_COMMIT_REF_NAME"):
                return os.environ["CI_COMMIT_REF_NAME"]
        else:
                try:
                        return subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
                except Exception:
                     return "local-only"

def Build():
        global outputDir
        command = ""
        outputDir = "out-x86_64"
        if platform.system() == "Windows":
                command = ['build/ninja', '-C']
                os.chdir(buildDir)
        elif platform.system() == "Linux":
                command = ['build/ninja-linux64', '-C']
        if (arch == "armv7l"):
                outputDir = "out-armv7l"
        command.append(outputDir)
        print(command)
        subprocess.check_call(command)

def GenerateCompileCommands():
        if arch == "x86_64":
                commandName = ""
                if platform.system() == "Windows":
                        commandName = 'build/ninja'
                        os.chdir(buildDir)
                elif platform.system() == "Linux":
                        commandName = 'build/ninja-linux64'
                        compileDB = subprocess.check_output([commandName, "-C", outputDir, '-t', 'compdb', 'cxx', 'cc'])
                with open(os.getcwd() + "/compile_commands.json", "w+") as compcmds:
                        compcmds.write(compileDB)

def GenerateBuildInfo():
        now = datetime.now()
        d1 = date(2018, 07, 20)
        d0 = date.today()

        delta = d0 - d1
        seconds_since_midnight = (now - now.replace(hour=0, minute=0, second=0, microsecond=0)).total_seconds()
        branchName = GetBranchName()
        version = str(delta.days) + "-" + str(int(seconds_since_midnight))
        with open('build/BuildInfo.h.in', 'r') as template:
                data = template.read().replace("##BUILD##", version)
                data = data.replace("##BRANCHNAME##", branchName.strip())
                data = data.replace("##YEAR##", str(datetime.now().year))
                data = data.replace("##ROOT_NAMESPACE##", rootNamespace)
                data = data.replace("##CLIENT_ID##", ClientID)
                data = data.replace("##CLIENT_KEY##", ClientKey)
                data = data.replace("##URL_PREFIX##", URLPrefix)
                data = data.replace("##WS_URL##", WSURLPrefix)
        with open("src/BuildInfo.h", 'w+') as buildInfo:
                buildInfo.write(data)
        with open("out-"+arch+"/VERSION", 'w+') as verInfo:
                verInfo.write(version)

GetServiceDetails()
InitBuildEnv()
GenerateBuildInfo()
os.chdir(buildDir)
Build()
GenerateCompileCommands()