import os
import sys
import platform
import subprocess
import shutil


def main():
	if len(sys.argv) < 3:
		print 'Missing Argument!'
		print 'Correct Usage:'
		print 'python SubmitRelease_Steam.py platform (windows, linux, macos or android) type (independent, oculus or steam)'
		return

	supportedPlatforms = ['windows', 'linux', 'macos', 'android', 'test']
	if not sys.argv[1] in supportedPlatforms:
		print 'Platform (' + sys.argv[1] + ') not supported!'
		return

	supportedTypes = ['independent', 'oculus', 'steam', 'test']
	if not sys.argv[2] in supportedTypes:
		print 'Build type (' + sys.argv[2] + ') not supported!'
		return

	currentDirectory = os.path.dirname(sys.argv[0])
	if currentDirectory:
		os.chdir(currentDirectory)

	sourceDirectory = os.path.join('../Builds/', sys.argv[1]+'_'+sys.argv[2])
	if sys.argv[1] == 'android':
		sourceDirectory = os.path.join(sourceDirectory, 'app/build/outputs/apk/release')
	elif sys.argv[1] == 'linux':
		sourceDirectory = os.path.join(sourceDirectory, 'Build')
	else:
		sourceDirectory = os.path.join(sourceDirectory, 'Build/ProjectSteve/Release')
	
	if not os.path.isdir(sourceDirectory):
		print "Build does not exist: " + sourceDirectory
		return

	destinationDirectory = os.path.join('../Releases/', sys.argv[1]+'_'+sys.argv[2])
	if os.path.isdir(destinationDirectory):
		shutil.rmtree(destinationDirectory, ignore_errors=True)

	if sys.argv[2] == 'independent':
		if sys.argv[1] == 'windows':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=shutil.ignore_patterns('*.lib', '*.exp', '../Builds/Steam/Build/Release/ProjectSteve/RayneOgg.dll', 'RayneBullet', 'RayneOculus', 'RayneOpenAL', 'RayneVR', 'RayneOpenVR'))
		elif sys.argv[1] == 'linux':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=None)
		elif sys.argv[1] == 'macos':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=shutil.ignore_patterns('.DS_STORE'))
			shutil.copy2('../Other/RunWithSteamVR-macOS.sh', destinationDirectory)
			shutil.copy2('../Other/RunWithSteamVRAndPreview-macOS.sh', destinationDirectory)
			shutil.copy2('../Other/RunPancake-macOS.sh', destinationDirectory)
		elif sys.argv[1] == 'android':
			os.makedirs(destinationDirectory)
			shutil.copy2(os.path.join(sourceDirectory, 'project-z.apk'), os.path.join(destinationDirectory, 'project-z-sidequest.apk'))
	elif sys.argv[2] == 'steam':
		if sys.argv[1] == 'windows':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=shutil.ignore_patterns('*.lib', '*.exp', '../Builds/Steam/Build/Release/ProjectSteve/RayneOgg.dll', 'RayneBullet', 'RayneOculus', 'RayneOpenAL', 'RayneVR', 'RayneOpenVR'))
		elif sys.argv[1] == 'linux':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=None)
		elif sys.argv[1] == 'macos':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=shutil.ignore_patterns('.DS_STORE'))
	elif sys.argv[2] == 'oculus':
		if sys.argv[1] == 'windows':
			shutil.copytree(sourceDirectory, destinationDirectory, symlinks=False, ignore=shutil.ignore_patterns('*.lib', '*.exp', '../Builds/Steam/Build/Release/ProjectSteve/RayneOgg.dll', 'RayneBullet', 'RayneOculus', 'RayneOpenAL', 'RayneVR', 'RayneOpenVR'))
		elif sys.argv[1] == 'android':
			os.makedirs(destinationDirectory)
			shutil.copy2(os.path.join(sourceDirectory, 'project-z.apk'), destinationDirectory)

if __name__ == '__main__':
	main()
