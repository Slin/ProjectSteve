import os
import sys
import platform
import urllib
import zipfile
import subprocess


def downloadButler():
	butlerFile = '../Vendor/itchio/butler'
	if platform.system() == 'Windows':
		butlerFile = '../Vendor/itchio/butler.exe'

	if os.path.isfile(butlerFile):
		return butlerFile

	if not os.path.exists('../Vendor/itchio'):
		os.makedirs('../Vendor/itchio')

	butlerDownloadURL = None
	if platform.system() == 'Darwin':
		butlerDownloadURL = 'https://broth.itch.ovh/butler/darwin-amd64/LATEST/archive/default'
	elif platform.system() == 'Windows':
		butlerDownloadURL = 'https://broth.itch.ovh/butler/windows-amd64/LATEST/archive/default'
	elif platform.system() == 'Linux':
		butlerDownloadURL = 'https://broth.itch.ovh/butler/linux-amd64/LATEST/archive/default'
	else:
		print 'Platform ' + platform.system() + ' not supported.'
		return None

	urllib.urlretrieve(butlerDownloadURL, '../Vendor/itchio/butler.zip')

	zippedFile = zipfile.ZipFile('../Vendor/itchio/butler.zip', 'r')
	zippedFile.extractall('../Vendor/itchio/')
	zippedFile.close()

	if os.path.exists('../Vendor/itchio/butler.zip'):
		os.remove('../Vendor/itchio/butler.zip')

	os.chmod(butlerFile, 0o775)

	return butlerFile


def main():
	if len(sys.argv) < 2:
		print 'Missing Argument!'
		print 'Correct Usage:'
		print 'python SubmitRelease_itchio.py platform (windows, linux, android or macos)'
		return

	supportedPlatforms = ['windows', 'linux', 'android', 'macos', 'test']
	if not sys.argv[1] in supportedPlatforms:
		print 'Platform (' + sys.argv[1] + ') not supported!'
		return

	currentDirectory = os.path.dirname(sys.argv[0])
	if currentDirectory:
		os.chdir(currentDirectory)

	butlerFile = downloadButler()
	if not butlerFile:
		return

	subprocess.call([butlerFile, 'upgrade'])
	subprocess.call([butlerFile, 'login'])

	if sys.argv[1] == 'windows':
		subprocess.call([butlerFile, 'push', '../Releases/windows_independent', 'slin/project-z:windows'])
	elif sys.argv[1] == 'linux':
		subprocess.call([butlerFile, 'push', '../Releases/linux_independent', 'slin/project-z:linux'])
	elif sys.argv[1] == 'android':
		subprocess.call([butlerFile, 'push', '../Releases/android_independent', 'slin/project-z:sidequest'])
	elif sys.argv[1] == 'macos':
		subprocess.call([butlerFile, 'push', '../Releases/macos_independent', 'slin/project-z:macos'])


if __name__ == '__main__':
	main()
