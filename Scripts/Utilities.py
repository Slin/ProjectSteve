import os
import sys
import platform


def setGradleProperty(file, key, value):
	oldData = None
	with open(file, "r") as f:
		oldData = f.read()

	lines = oldData.splitlines()
	properties = list()

	didOverwriteProperty = 0
	for line in lines:
		keyValue = line.split('=', 1)
		if len(keyValue) > 1:
			if keyValue[0] == key:
				keyValue[1] = value
				didOverwriteProperty = 1
		properties.append(keyValue)

	if didOverwriteProperty == 0:
		keyValue = list()
		keyValue.append(key)
		keyValue.append(value)
		properties.append(keyValue)

	with open(file, "w") as f:
		for keyValue in properties:
			if len(keyValue) > 1:
				f.write(keyValue[0]+'='+keyValue[1]+'\n')
			else:
				f.write(keyValue[0]+'\n')
	
def getBuildNumber(file):
	oldData = None
	with open(file, "r") as f:
		oldData = f.read()
	lines = oldData.splitlines()
	print oldData
	if len(lines) < 1:
		return 0
	return int(lines[0])

def getVersion(file):
	oldData = None
	with open(file, "r") as f:
		oldData = f.read()
	lines = oldData.splitlines()
	if len(lines) < 2:
		return '0.0.0'
	return lines[1]

def setVersionAndBuildNumber(file, version, buildNumber):
	lines = list()
	lines.append(str(buildNumber))
	lines.append(version)

	with open(file, "w") as f:
		for line in lines:
			f.write(line+'\n')
