#!/usr/bin/python
import os
import sys
import subprocess

targetOutDir = sys.argv[3]
rootDir = sys.argv[1]
outputDir = sys.argv[2]
inputFiles = sys.argv[4]
genType = sys.argv[5]

def FindAndLaunchProtoc():
    protoCPath = subprocess.check_output("which protoc", shell=True)
    if protoCPath == "":
        print "Please download and compile the Protobuf compiler (protoc) and put in your PATH"
        exit(1)
    else:
        GenerateFiles(protoCPath.replace('\n', ""))

def GenerateFiles(path):
    outputPath = os.path.abspath(outputDir);
    if not os.path.exists(outputPath):
        os.makedirs(outputPath)

    originalBuildDir = os.getcwd()
    inputFile = os.path.abspath(inputFiles);

    copyCCToOutDir = "cp " + outputPath + "/*.cc" + " " + originalBuildDir + "/" + targetOutDir
    copyHToOutDir = "cp " + outputPath + "/*.h" + " " + originalBuildDir + "/" + targetOutDir

    outputAndInputPath = outputPath + " " + inputFile#s.replace("../build/protocols/", "")

    protoCGenPath = path + " --proto_path=" + outputPath.replace("/generated", "") + " --cpp_out=" + outputAndInputPath

    subprocess.check_call(protoCGenPath, shell=True)
    if genType == "grpc":
        grpcPluginPath = os.path.abspath(outputPath + "../../../../")
        grpcGenPath = path + " --proto_path=" + outputPath.replace("/generated", "") + " --grpc_out=" + outputPath + ' --plugin=protoc-gen-grpc="' + grpcPluginPath + '/build/grpc_cpp_plugin" ' + inputFile
        subprocess.check_call(grpcGenPath, shell=True)

    subprocess.check_call(copyCCToOutDir, shell=True)
    subprocess.check_call(copyHToOutDir, shell=True)

FindAndLaunchProtoc()