//
//  PSPlatformWrapper.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSPlatformWrapper.h"
#include "PSWorld.h"

#if defined(BUILD_FOR_STEAM)
#include "steam_api.h"
#elif defined(BUILD_FOR_OCULUS)
#include "OVR_Platform.h"
#if RN_PLATFORM_WINDOWS
#include "OVR_PlatformLoader.cpp"
#endif
#endif

namespace PS
{
	void Platform::Initialize()
	{
#if defined(BUILD_FOR_STEAM)
		if(!SteamAPI_Init())
		{
			World::Exit();
		}

		// Is Steam loaded? If not we can't get stats.
		if(!SteamUserStats() || !SteamUser()/* || !SteamUser()->BLoggedOn()*/)
		{
			World::Exit();
		}

		// Request user stats.
		SteamUserStats()->RequestCurrentStats();
/*#elif defined(BUILD_FOR_OCULUS)
#if RN_PLATFORM_WINDOWS
		if(ovr_PlatformInitializeWindows("1788378381209253") != ovrPlatformInitialize_Success)
		{
			World::Exit();
		}
		ovr_Entitlement_GetIsViewerEntitled();

#elif RN_PLATFORM_ANDROID
		android_app *androidApp = RN::Kernel::GetSharedInstance()->GetAndroidApp();
		JNIEnv* jni;
        androidApp->activity->vm->AttachCurrentThread(&jni, NULL);
		if(ovr_PlatformInitializeAndroid("2129647710395917", androidApp->activity->clazz, jni) != ovrPlatformInitialize_Success)
        {
        	androidApp->activity->vm->DetachCurrentThread();
        	World::Exit();
        }
        //androidApp->activity->vm->DetachCurrentThread();
        ovr_Entitlement_GetIsViewerEntitled();
#endif*/
#endif
	}

	void Platform::Shutdown()
	{
#if defined(BUILD_FOR_STEAM)
		SteamAPI_Shutdown();
#endif
	}

	void Platform::HandleEvents()
	{
#if defined(BUILD_FOR_STEAM)
		SteamAPI_RunCallbacks();
/*#elif defined(BUILD_FOR_OCULUS)
		ovrMessageHandle message;
		while((message = ovr_PopMessage()) != nullptr)
		{
			switch(ovr_Message_GetType(message))
			{
			case ovrMessage_Entitlement_GetIsViewerEntitled:
				if(!ovr_Message_IsError(message))
				{
					// User is entitled.  Continue with normal game behaviour
				}
				else
				{
					World::Exit();
				}
				break;

			case ovrMessage_User_GetAccessToken:
				if(!ovr_Message_IsError(message))
				{
					RNDebug("Oculus User Access Token: " << ovr_Message_GetString(message));
				}
				break;

			default:
				break;
			}
		}*/
#endif
	}

	void Platform::UnlockAchievement(const char *name)
	{
#if defined(BUILD_FOR_STEAM)
		SteamUserStats()->SetAchievement(name);
		SteamUserStats()->StoreStats();
#elif defined(BUILD_FOR_OCULUS)
//		ovr_Achievements_Unlock(name);
#endif
	}

	void Platform::ResetAllAchievements()
	{
#if defined(BUILD_FOR_STEAM)
		SteamUserStats()->ResetAllStats(true);
		SteamUserStats()->StoreStats();
#endif
	}
}
