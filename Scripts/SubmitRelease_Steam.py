import os
import sys
import platform
import subprocess


def main():
	if len(sys.argv) < 2:
		print 'Missing Argument!'
		print 'Correct Usage:'
		print 'python SubmitRelease_Steam.py platform (windows, linux or macos)'
		return

	supportedPlatforms = ['windows', 'linux', 'macos', 'test']
	if not sys.argv[1] in supportedPlatforms:
		print 'Platform (' + sys.argv[1] + ') not supported!'
		return

	currentDirectory = os.path.dirname(sys.argv[0])
	if currentDirectory:
		os.chdir(currentDirectory)

	builderFile = '../Vendor/Steamworks/tools/ContentBuilder/builder_osx/steamcmd.sh'
	if platform.system() == 'Windows':
		builderFile = '../Vendor/Steamworks/tools/ContentBuilder/builder/steamcmd.exe'
	elif platform.system() == 'Linux':
		builderFile = '../Vendor/Steamworks/tools/ContentBuilder/builder_linux/steamcmd.sh'

	if sys.argv[1] == 'windows':
		subprocess.call([builderFile, '+login', 'slindev', '+run_app_build_http', os.path.join(currentDirectory, 'app_build_919730_Win64.vdf'), '+quit'])
	elif sys.argv[1] == 'linux':
		subprocess.call([builderFile, '+login', 'slindev', '+run_app_build_http', os.path.join(currentDirectory, 'app_build_919730_Linux64.vdf'), '+quit'])
	elif sys.argv[1] == 'macos':
		subprocess.call([builderFile, '+login', 'slindev', '+run_app_build_http', os.path.join(currentDirectory, 'app_build_919730_MacOS.vdf'), '+quit'])


if __name__ == '__main__':
	main()
