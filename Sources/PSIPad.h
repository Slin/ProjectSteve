//
//  PZLevelSelectorScreen.h
//  Project: Z
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_IPAD_H_
#define __ProjectSteve_IPAD_H_

#include <Rayne.h>
#include "RNPhysXWorld.h"
#include "RNOpenALWorld.h"
#include "Sources/RNUI.h"
#include "PSGrabbable.h"

namespace PS
{
	class IPad : public Grabbable
	{
	public:
		IPad();
		~IPad();
		
		void CreateUI();
		void Update(float delta) override;

	private:
		void UpdateCursorPosition();
		void UpdateUI(float delta);

		bool _needsRedraw;
		
		bool _isAnimating;
		bool _wasActivating;
		
		RN::OpenALSource *_audioSource;
		RN::UI::Window *_uiWindow;
		RN::Material *_screenMaterial;
		RN::Entity *_screenEntity;
		
		bool _isCursorActive;
		RN::Entity *_cursorEntity;
		RN::Vector2 _cursorPosition;
		
		RN::PhysXDynamicBody *_physicsBody;
		
		RN::UI::ScrollView *_levelScrollView;
		
		RNDeclareMeta(Grabbable)
	};
}

#endif /* defined(__ProjectSteve_IPAD_H_) */
