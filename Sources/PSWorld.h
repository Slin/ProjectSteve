//
//  PSWorld.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_WORLD_H_
#define __ProjectSteve_WORLD_H_

#include <Rayne.h>

#include "RNVRCamera.h"
#include "RNPhysXWorld.h"
#include "RNOpenALWorld.h"
#include "RNRecastWorld.h"

#include "PSPlayer.h"

namespace PS
{
	class World : public RN::SceneBasic
	{
	public:
		enum CollisionType
		{
			Level = 1 << 0,
			Players = 1 << 1,
			
			None = 0x0,
			All = 0xffffffff
		};
		
		static World *GetSharedInstance();
		static void Exit();

		World(RN::VRWindow *vrWindow, RN::uint8 msaa, bool wantsPreview, bool debug);

		RN::PhysXWorld *GetPhysicsWorld() const { return _physicsWorld; }
		RN::OpenALWorld *GetAudioWorld() const { return _audioWorld; }
		RN::RecastWorld *GetNavigationWorld() const { return _navigationWorld; }
		RN::ShaderLibrary *GetShaderLibrary() const { return _shaderLibrary; }
		RN::SceneNode *GetCamera() const { return _mainCamera; }
		Player *GetPlayer() const { return _player; }

		void AddLevelNode(RN::SceneNode *node);
		void RemoveLevelNode(RN::SceneNode *node);
		
		bool IsPaused() const { return _isPaused; }
		bool IsDash() const { return _isDash; }
		
		RN::Entity *_levelEntity;

	protected:
		void WillBecomeActive() override;
		void WillUpdate(float delta) override;
		void DidUpdate(float delta) override;
		
		void InitializePlatform();
		
		void UpdateForWindowSize() const;
		
		void ClearLevel();
		void LoadLevel();

		RN::ShaderLibrary *_shaderLibrary;
		RN::Array *_levelNodes;

		Player *_player;
		RN::Camera *_shadowCamera;
		RN::VRCamera *_vrCamera;
		RN::SceneNode *_mainCamera;
		RN::VRWindow *_vrWindow;
		RN::uint8 _msaa;
		bool _wantsPreviewWindow;
		bool _debug;
		
		bool _escWasPressed;

		RN::Window *_previewWindow;
		RN::Material *_copyEyeToScreenMaterial;

		RN::PhysXWorld *_physicsWorld;
		RN::OpenALWorld *_audioWorld;
		RN::RecastWorld *_navigationWorld;
		
		bool _isPaused;
		bool _isDash;

		static World *_sharedInstance;
	};
}


#endif /* __ProjectSteve_WORLD_H_ */
