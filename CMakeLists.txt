cmake_minimum_required(VERSION 2.8.4)
project(ProjectSteve)

if(ANDROID)
	option(RN_BUILD_OCULUSMOBILE_MODULE "Build the Oculus GearVR module" ON)
	option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" ON)
elseif(UNIX AND NOT APPLE)
	option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" ON)
	option(RN_BUILD_OCULUS_MODULE "Build the Oculus module" OFF)
	option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
elseif(APPLE)
	option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" OFF)
	option(RN_BUILD_OCULUS_MODULE "Build the Oculus module" OFF)
	option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
elseif(MSVC)
	option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" OFF)
	option(RN_BUILD_D3D12_MODULE "Build the D3D12 renderer module" ON)
	option(RN_BUILD_OCULUS_MODULE "Build the Oculus module" ON)
	option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
else()
	option(RN_BUILD_D3D12_MODULE "Build the D3D12 renderer module" OFF)
	option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" ON)
	option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
endif()

option(RN_BUILD_BULLET_MODULE "Build the Bullet physics module" OFF)
option(RN_BUILD_PHYSX_MODULE "Build the PhysX physics module" ON)
option(RN_BUILD_ASSIMP_MODULE "Build the Assimp module" OFF)
option(RN_BUILD_OGG_MODULE "Build the ogg audio file module" ON)
option(RN_BUILD_STEAMAUDIO_MODULE "Build the Steam Audio module" OFF)
option(RN_BUILD_OPENAL_MODULE "Build the OpenAL module" ON)
option(RN_BUILD_ENET_MODULE "Build the ENet module" OFF)
option(RN_BUILD_RECAST_MODULE "Build the Recast pathfinding module" ON)
option(RN_BUILD_SAMPLES "Enable building the sample projects" OFF)
option(RN_BUILD_UI_MODULE "Build the UI module" ON)

if(WIN32)
	set(CMAKE_SYSTEM_VERSION "10.0.10240.0")
	option(PZ_BUILD_FOR_OCULUS "Enables the Oculus Platform SDK and excludes OpenVR and Steamworks" ON)
	option(PZ_BUILD_FOR_STEAM "Enables the Steamworks SDK and excludes the Oculus Platform SDK" OFF)
elseif(ANDROID)
	option(PZ_BUILD_FOR_OCULUS "Enables the Oculus Platform SDK and excludes OpenVR and Steamworks" ON)
	option(PZ_BUILD_FOR_STEAM "Enables the Steamworks SDK and excludes the Oculus Platform SDK" OFF)
else()
	option(PZ_BUILD_FOR_OCULUS "Enables the Oculus Platform SDK and excludes OpenVR and Steamworks" OFF)
	option(PZ_BUILD_FOR_STEAM "Enables the Steamworks SDK and excludes the Oculus Platform SDK" ON)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Build)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(APPLE)
	set(CMAKE_INSTALL_RPATH @executable_path;@executable_path/../Resources/Resources/Modules/RaynePhysX;@executable_path/../Resources/Resources/Modules/RayneMetal;@executable_path/../Resources/Resources/Modules/RayneOgg;@executable_path/../Resources/Resources/Modules/RayneOpenAL;@executable_path/../Resources/Resources/Modules/RayneRecast;@executable_path/../Resources/Resources/Modules/RayneOpenVR;@executable_path/../Resources/Resources/Modules/RayneVR;@executable_path/../Resources/Resources/Modules/RayneUI)
elseif(UNIX)
	set(CMAKE_INSTALL_RPATH \$ORIGIN;\$ORIGIN/Resources/Modules/RaynePhysX;\$ORIGIN/Resources/Modules/RayneMetal;\$ORIGIN/Resources/Modules/RayneOgg;\$ORIGIN/Resources/Modules/RayneOpenAL;\$ORIGIN/Resources/Modules/RayneRecast;\$ORIGIN/Resources/Modules/RayneOpenVR;\$ORIGIN/Resources/Modules/RayneVR;\$ORIGIN/Resources/Modules/RayneUI)
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../Rayne" "${CMAKE_CURRENT_BINARY_DIR}/Rayne")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../Rayne/CMake/Rayne.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCES
	Sources/PSApplication.cpp
	Sources/PSPlatformWrapper.cpp
	Sources/PSWorld.cpp
	Sources/PSPlayer.cpp
	Sources/main.cpp
	Sources/PSStevelet.cpp
	Sources/PSAnimatable.cpp
	Sources/PSHelix.cpp
	Sources/PSSyringe.cpp
	Sources/PSObstacle.cpp
	Sources/PSLevel.cpp
	Sources/PSHelix.cpp
	Sources/PSGrabbable.cpp)

set(HEADERS
	Sources/PSApplication.h
	Sources/PSPlatformWrapper.h
	Sources/PSWorld.h
	Sources/PSPlayer.h
	Sources/PSStevelet.h
	Sources/PSSteveStats.h
	Sources/PSAnimatable.h
	Sources/PSHelix.h
	Sources/PSSyringe.h
	Sources/PSObstacle.h
	Sources/PSLevel.h
	Sources/PSGrabbable.h)

set(RESOURCES
	manifest.json
	Resources)

if(ANDROID)
    add_library(ProjectSteve SHARED "${SOURCES}" "${HEADERS}")
else()
    add_executable(ProjectSteve "${SOURCES}" "${HEADERS}")
endif()

rayne_copy_resources(ProjectSteve "${RESOURCES}")

rayne_link_with(ProjectSteve)
rayne_use_modules(ProjectSteve RaynePhysX)
rayne_use_modules(ProjectSteve RayneOpenAL)
rayne_use_modules(ProjectSteve RayneOgg)
rayne_use_modules(ProjectSteve RayneRecast)
rayne_use_modules(ProjectSteve RayneVR)
rayne_use_modules(ProjectSteve RayneUI)

if(ANDROID)
    rayne_use_modules(ProjectSteve RayneVulkan)
    rayne_use_modules(ProjectSteve RayneOculusMobile)

    target_compile_definitions(ProjectSteve PRIVATE -DBUILD_FOR_OCULUS)
	target_include_directories(ProjectSteve PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OculusPlatform/Include)
	if(PZ_BUILD_FOR_OCULUS)
    	target_link_libraries(ProjectSteve ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OculusPlatform/Android/libs/arm64-v8a/libovrplatformloader.so)
    	add_custom_command(TARGET ProjectSteve PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OculusPlatform/Android/libs/arm64-v8a/libovrplatformloader.so $<TARGET_FILE_DIR:ProjectSteve>)
    	target_compile_definitions(ProjectSteve PRIVATE -DBUILD_FOR_OCULUS)
    endif()
elseif(UNIX AND NOT APPLE)
	rayne_use_modules(ProjectSteve RayneVulkan)
	rayne_use_modules(ProjectSteve RayneOpenVR)
else()
	if(NOT APPLE)
			if(${RN_BUILD_VULKAN_MODULE})
				rayne_use_modules(ProjectSteve RayneVulkan)
			endif()

			if(WIN32 AND ${RN_BUILD_D3D12_MODULE})
				rayne_use_modules(ProjectSteve RayneD3D12)
				rayne_use_modules(ProjectSteve RayneOculus)
			endif()

    		add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:OpenAL>" "$<TARGET_FILE_DIR:ProjectSteve>/$<TARGET_FILE_NAME:OpenAL>")
	else()
	    rayne_use_modules(ProjectSteve RayneMetal)
	endif()

	if(PZ_BUILD_FOR_STEAM)
	    rayne_use_modules(ProjectSteve RayneOpenVR)
	    target_compile_definitions(ProjectSteve PRIVATE -DBUILD_FOR_STEAM)
	    target_include_directories(ProjectSteve PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Steamworks/public/steam)
	    if(NOT APPLE)
	        target_link_libraries(ProjectSteve ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Steamworks/redistributable_bin/win64/steam_api64.lib)
	        add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Steamworks/redistributable_bin/win64/steam_api64.dll $<TARGET_FILE_DIR:ProjectSteve>/steam_api64.dll)
	        add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/steam_appid.txt $<TARGET_FILE_DIR:ProjectSteve>/steam_appid.txt)
	    else()
	        target_link_libraries(ProjectSteve ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Steamworks/redistributable_bin/osx/libsteam_api.dylib)
	        add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Steamworks/redistributable_bin/osx/libsteam_api.dylib $<TARGET_FILE_DIR:ProjectSteve>/libsteam_api.dylib)
	        add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/steam_appid.txt $<TARGET_BUNDLE_CONTENT_DIR:ProjectSteve>/../../steam_appid.txt)
		endif()
	elseif(PZ_BUILD_FOR_OCULUS)
	    target_compile_definitions(ProjectSteve PRIVATE -DBUILD_FOR_OCULUS)
	    target_include_directories(ProjectSteve PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OculusPlatform/Windows ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OculusPlatform/Include)
	    target_link_libraries(ProjectSteve ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OculusPlatform/Windows/LibOVRPlatform64_1.lib)
	else()
	    rayne_use_modules(ProjectSteve RayneOpenVR)
	    if(NOT APPLE)
		    add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunWithOculus-windows.bat $<TARGET_FILE_DIR:ProjectSteve>/RunWithOculus-windows.bat)
		    add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunWithSteamVR-windows.bat $<TARGET_FILE_DIR:ProjectSteve>/RunWithSteamVR-windows.bat)
		    add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunPancake-windows.bat $<TARGET_FILE_DIR:ProjectSteve>/RunPancake-windows.bat)
		else()
			add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunWithSteamVRAndPreview-macOS.sh $<TARGET_BUNDLE_CONTENT_DIR:ProjectSteve>/../../RunWithSteamVRAndPreview-macOS.sh)
		    add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunWithSteamVR-macOS.sh $<TARGET_BUNDLE_CONTENT_DIR:ProjectSteve>/../../RunWithSteamVR-macOS.sh)
		    add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunPancake-macOS.sh $<TARGET_BUNDLE_CONTENT_DIR:ProjectSteve>/../../RunPancake-macOS.sh)
		endif()
	endif()
endif()

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProjectSteve)
endif()

if(APPLE)
	set_target_properties(ProjectSteve PROPERTIES MACOSX_BUNDLE TRUE)
	set_target_properties(ProjectSteve PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Other/Info.plist.in)
	set_target_properties(ProjectSteve PROPERTIES MACOSX_BUNDLE_ICON_FILE icon)

	add_custom_command(TARGET ProjectSteve POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/icon.icns $<TARGET_BUNDLE_CONTENT_DIR:ProjectSteve>/Resources/icon.icns)
endif()
