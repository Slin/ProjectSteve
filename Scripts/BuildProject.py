import os
import sys
import platform
import subprocess
import shutil
import getpass
import Utilities


def main():
	if len(sys.argv) < 3:
		print 'Missing Argument!'
		print 'Correct Usage:'
		print 'python BuildProject.py platform (windows, linux, macos or android) type (independent, oculus or steam)'
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

	buildNumber = Utilities.getBuildNumber('../VERSION')+1
	versionString = Utilities.getVersion('../VERSION')
	Utilities.setVersionAndBuildNumber('../VERSION', versionString, buildNumber)

	buildDirectory = os.path.join('../Builds/', sys.argv[1]+'_'+sys.argv[2])
	os.chdir(buildDirectory)

	if sys.argv[2] == 'independent':
		if sys.argv[1] == 'windows':
			print 'Not yet supported'
		elif sys.argv[1] == 'linux':
			subprocess.call(['make'])
		elif sys.argv[1] == 'macos':
			print '"Set :CFBundleShortVersionString '+versionString+'"'
			subprocess.call(['/usr/libexec/PlistBuddy', '-c', 'Set :CFBundleShortVersionString '+versionString, 'CMakeFiles/ProjectSteve.dir/Info.plist'])
			subprocess.call(['/usr/libexec/PlistBuddy', '-c', 'Set :CFBundleVersion '+str(buildNumber), 'CMakeFiles/ProjectSteve.dir/Info.plist'])
			subprocess.call(['xcodebuild', '-project', 'ProjectSteve.xcodeproj', '-target', 'ProjectSteve', '-configuration', 'Release', 'build'])
		elif sys.argv[1] == 'android':
			Utilities.setGradleProperty('gradle.properties', 'projectVersion', versionString)
			Utilities.setGradleProperty('gradle.properties', 'projectBuildNumber', str(buildNumber))
			print 'Keystore password?'
			storePassword = getpass.getpass()
			print 'Key password?'
			keyPassword = getpass.getpass()
			subprocess.call(['gradle', 'assembleRelease'])
			subprocess.call(['jarsigner', '-keystore', os.path.join(currentDirectory, '../../../AndroidReleaseKeyStore'), '-storepass', storePassword, 'app/build/outputs/apk/release/app-release-unsigned.apk', 'AndroidReleaseKey', '-keypass', keyPassword])
			subprocess.call(['/Users/slin/Library/Android/sdk/build-tools/29.0.2/zipalign', '-f', '4', 'app/build/outputs/apk/release/app-release-unsigned.apk', 'app/build/outputs/apk/release/project-z.apk'])
	elif sys.argv[2] == 'steam':
		if sys.argv[1] == 'windows':
			print 'Not yet supported'
		elif sys.argv[1] == 'linux':
			subprocess.call(['make'])
		elif sys.argv[1] == 'macos':
			subprocess.call(['/usr/libexec/PlistBuddy', '-c', 'Set :CFBundleShortVersionString '+versionString, 'CMakeFiles/ProjectSteve.dir/Info.plist'])
			subprocess.call(['/usr/libexec/PlistBuddy', '-c', 'Set :CFBundleVersion '+str(buildNumber), 'CMakeFiles/ProjectSteve.dir/Info.plist'])
			subprocess.call(['xcodebuild', '-project', 'ProjectSteve.xcodeproj', '-target', 'ProjectSteve', '-configuration', 'Release', 'build'])
	elif sys.argv[2] == 'oculus':
		if sys.argv[1] == 'windows':
			print 'Not yet supported'
		elif sys.argv[1] == 'android':
			Utilities.setGradleProperty('gradle.properties', 'projectVersion', versionString)
			Utilities.setGradleProperty('gradle.properties', 'projectBuildNumber', str(buildNumber))
			print 'Keystore password?'
			storePassword = getpass.getpass()
			print 'Key password?'
			keyPassword = getpass.getpass()
			subprocess.call(['gradle', 'assembleRelease'])
			subprocess.call(['jarsigner', '-verbose', '-keystore', os.path.join(currentDirectory, '../../../AndroidReleaseKeyStore'), '-storepass', storePassword, 'app/build/outputs/apk/release/app-release-unsigned.apk', 'AndroidReleaseKey', '-keypass', keyPassword])
			subprocess.call(['/Users/slin/Library/Android/sdk/build-tools/29.0.2/zipalign', '-f', '4', 'app/build/outputs/apk/release/app-release-unsigned.apk', 'app/build/outputs/apk/release/project-z.apk'])

if __name__ == '__main__':
	main()
