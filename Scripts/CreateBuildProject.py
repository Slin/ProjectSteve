import os
import sys
import platform
import subprocess
import shutil
import distutils.dir_util
import Utilities


def copyToFolder(file, folder):
	if not os.path.isdir(folder):
		os.makedirs(folder)
	shutil.copy2(file, folder)

def main():
	if len(sys.argv) < 3:
		print 'Missing Argument!'
		print 'Correct Usage:'
		print 'python CreateBuildProject.py platform (windows, linux, macos or android) type (independent, oculus or steam)'
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

	buildDirectory = os.path.join('../Builds/', sys.argv[1]+'_'+sys.argv[2])
	if os.path.isdir(buildDirectory):
		shutil.rmtree(buildDirectory, ignore_errors=True)

	os.makedirs(buildDirectory)
	os.chdir(buildDirectory)

	print os.getcwd()

	if sys.argv[2] == 'independent':
		if sys.argv[1] == 'windows':
			subprocess.call(['cmake', '-A', 'x64', '../..', '-DPZ_BUILD_FOR_STEAM=False', '-DPZ_BUILD_FOR_OCULUS=False'])
		elif sys.argv[1] == 'linux':
			subprocess.call(['cmake', '../..', '-DPZ_BUILD_FOR_STEAM=False', '-DPZ_BUILD_FOR_OCULUS=False', '-DCMAKE_BUILD_TYPE=Release'])
		elif sys.argv[1] == 'macos':
			subprocess.call(['cmake', '-G', 'Xcode', '../..', '-DPZ_BUILD_FOR_STEAM=False', '-DPZ_BUILD_FOR_OCULUS=False'])
		elif sys.argv[1] == 'android':
			subprocess.call(['gradle', 'init', '--type', 'basic', '--dsl', 'groovy', '--project-name', 'ProjectSteve'])
			distutils.dir_util.copy_tree('../../Other/android-buildsystem', '.')
			Utilities.setGradleProperty('gradle.properties', 'projectCmakeArguments', '-DCMAKE_BUILD_TYPE=Release, -DPZ_BUILD_FOR_STEAM=False, -DPZ_BUILD_FOR_OCULUS=False')
			copyToFolder('../../../../Rayne/Modules/OculusMobile/Vendor/ovr_sdk_mobile_1/VrApi/Libs/Android/arm64-v8a/Release/libvrapi.so', 'app/src/main/libs/arm64-v8a/')
			copyToFolder('../../../../Rayne/Modules/OculusMobile/Vendor/ovr_sdk_mobile_1/VrApi/Libs/Android/arm64-v8a/Debug/libvrapi.so', 'app/src/debug/libs/arm64-v8a/')
	elif sys.argv[2] == 'steam':
		if sys.argv[1] == 'windows':
			subprocess.call(['cmake', '-A', 'x64', '../..', '-DPZ_BUILD_FOR_STEAM=True', '-DPZ_BUILD_FOR_OCULUS=False'])
		elif sys.argv[1] == 'linux':
			subprocess.call(['cmake', '../..', '-DPZ_BUILD_FOR_STEAM=True', '-DPZ_BUILD_FOR_OCULUS=False', '-DCMAKE_BUILD_TYPE=Release', "-DCMAKE_C_COMPILER=/usr/bin/clang", "-DCMAKE_CXX_COMPILER=/usr/bin/clang++"])
		elif sys.argv[1] == 'macos':
			subprocess.call(['cmake', '-G', 'Xcode', '../..', '-DPZ_BUILD_FOR_STEAM=True', '-DPZ_BUILD_FOR_OCULUS=False'])
	elif sys.argv[2] == 'oculus':
		if sys.argv[1] == 'windows':
			subprocess.call(['cmake', '-A', 'x64', '../..', '-DPZ_BUILD_FOR_STEAM=False', '-DPZ_BUILD_FOR_OCULUS=True'])
		elif sys.argv[1] == 'android':
			subprocess.call(['gradle', 'init', '--type', 'basic', '--dsl', 'groovy', '--project-name', 'ProjectSteve'])
			subprocess.call(['gradle', '-PrayneBuildType=oculus', '-PrayneBuildVersion=1.0.7'])
			distutils.dir_util.copy_tree('../../Other/android-buildsystem', '.')
			Utilities.setGradleProperty('gradle.properties', 'projectCmakeArguments', '-DCMAKE_BUILD_TYPE=Release, -DPZ_BUILD_FOR_STEAM=False, -DPZ_BUILD_FOR_OCULUS=True')
			copyToFolder('../../../../Rayne/Modules/OculusMobile/Vendor/ovr_sdk_mobile_1/VrApi/Libs/Android/arm64-v8a/Release/libvrapi.so', 'app/src/main/libs/arm64-v8a/')
			copyToFolder('../../../../Rayne/Modules/OculusMobile/Vendor/ovr_sdk_mobile_1/VrApi/Libs/Android/arm64-v8a/Debug/libvrapi.so', 'app/src/debug/libs/arm64-v8a/')
			copyToFolder('../../Vendor/OculusPlatform/Android/libs/arm64-v8a/libovrplatformloader.so', 'app/src/main/libs/arm64-v8a/')
	

if __name__ == '__main__':
	main()
