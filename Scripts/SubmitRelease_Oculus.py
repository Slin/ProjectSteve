import os
import sys
import platform
import urllib
import zipfile
import subprocess


def downloadUtility():
	utilityFile = '../Vendor/OculusPlatform/ovr-platform-util.dms'
	if platform.system() == 'Windows':
		utilityFile = '../Vendor/OculusPlatform/ovr-platform-util.exe'

	if os.path.isfile(utilityFile):
		return utilityFile

	if not os.path.exists('../Vendor/OculusPlatform'):
		os.makedirs('../Vendor/OculusPlatform')

	utilityDownloadURL = None
	if platform.system() == 'Darwin':
		utilityDownloadURL = 'https://www.oculus.com/download_app/?id=1462426033810370'
	elif platform.system() == 'Windows':
		utilityDownloadURL = 'https://www.oculus.com/download_app/?id=1076686279105243'
	else:
		print 'Platform ' + platform.system() + ' not supported.'
		return None

	urllib.urlretrieve(utilityDownloadURL, utilityFile)
	os.chmod(utilityFile, 0o775)

	return utilityFile


def main():
	if len(sys.argv) < 2:
		print 'Missing Argument!'
		print 'Correct Usage:'
		print 'python SubmitRelease_Oculus.py platform (windows or android)'
		return

	supportedPlatforms = ['windows', 'android', 'test']
	if not sys.argv[1] in supportedPlatforms:
		print 'Platform (' + sys.argv[1] + ') not supported!'
		return

	currentDirectory = os.path.dirname(sys.argv[0])
	if currentDirectory:
		os.chdir(currentDirectory)

	utilityFile = downloadUtility()
	if not utilityFile:
		return

	if sys.argv[1] == 'windows':
		version = raw_input("What version? ")
		subprocess.call([utilityFile, 'upload-rift-build', '-a', '1788378381209253', '-s', 'e9bf04e9bc94bf4255409a5719419e3b', '-d', 'Releases\\Oculus\\ProjectSteve', '-l', 'ProjectSteve.exe', '-c', 'alpha', '-v', version, '-P', '--pancake', '-r', '1183534128364060'])
	elif sys.argv[1] == 'android':
		subprocess.call([utilityFile, 'upload-mobile-build', '--apk', '../Releases/android_oculus/project-z.apk', '-a', '2129647710395917', '-s', '726b2876e5a6a859f86d136a053b2c69', '-c', 'alpha'])


if __name__ == '__main__':
	main()
