#!/usr/bin/python
import os, sys, requests

def UploadFiles():
	version = ""
	headers = {
		'Authorization': 'BuildService '+ os.environ["UPDATE_KEY"]
	}
	files = [
		('file', open('out-armv7l/slideshow', 'rb')),
		('file', open('out-armv7l/libuikit.so', 'rb')),
		('file', open('out-armv7l/libbase.so', 'rb'))
	]

	with open("out-armv7l/VERSION", 'r+') as verInfo:
		version = verInfo.read()

	print("Uploading: " + version)

	updateInfo = {"version": version, "changes": os.environ["CI_COMMIT_MESSAGE"]}

	r = requests.put(os.environ["URL_PREFIX"]+"/loki/update/upload", files=files, headers=headers, data=updateInfo)
	x = r.json()
	if x["status"] == "success":
		print "Success!"
		sys.exit(0)
	else:
		print x
		sys.exit(1)

UploadFiles()