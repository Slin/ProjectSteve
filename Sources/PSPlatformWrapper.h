//
//  PSPlatformWrapper.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_PLATFORMWRAPPER_H_
#define __ProjectSteve_PLATFORMWRAPPER_H_

#include "Rayne.h"

namespace PS
{
	class Platform
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void HandleEvents();
		static void UnlockAchievement(const char *name);
		static void ResetAllAchievements();

	private:

	};
}


#endif /* __ProjectSteve_PLATFORMWRAPPER_H_ */
