#include <Rayne.h>
#include "PSApplication.h"

#if RN_BUILD_RELEASE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#if RN_PLATFORM_ANDROID
void android_main(struct android_app *app)
{
	RN::Initialize(0, nullptr, new PS::Application(), app);
}
#else
int main(int argc, const char *argv[])
{
#if RN_BUILD_DEBUG
//	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
#endif
	RN::Initialize(argc, argv, new PS::Application());
}
#endif
