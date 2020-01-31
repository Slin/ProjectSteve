import os
import sys
import platform
import urllib
import zipfile
import subprocess


def downloadAndExtractURL(downloadURL, directory):
	if not os.path.exists(directory):
		os.makedirs(directory)

	filePath = os.path.join(directory, 'download.zip')
	urllib.urlretrieve(downloadURL, filePath)

	zippedFile = zipfile.ZipFile(filePath, 'r')
	zippedFile.extractall(directory)
	zippedFile.close()

	if os.path.exists(filePath):
		os.remove(filePath)


def main():
	currentDirectory = os.path.dirname(sys.argv[0])
	if currentDirectory:
		os.chdir(currentDirectory)

	#downloadAndExtractURL('https://partner.steamgames.com/downloads/steamworks_sdk.zip', '../Vendor/Steamworks')
	downloadAndExtractURL('https://securecdn.oculus.com/binaries/download/?id=1329608803829485', '../Vendor/OculusPlatform')


if __name__ == '__main__':
	main()
