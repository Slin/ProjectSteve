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

namespace PS
{
	class Player;
	class Grabbable;
	class Helix;
	class Stevelet;
	class IPad;

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
		Helix *GetHelix() const { return _helix; }
		IPad* GetIPad() const { return _ipad; }
		
		Grabbable* GetClosestGrabbableObject(RN::Vector3 position);
		std::pair<Grabbable*,float> GetClosestGrabbableObject(RN::Vector2 coordinate);

		void AddLevelNode(RN::SceneNode *node, bool _isGrabbable);
		void RemoveLevelNode(RN::SceneNode *node);
		void RegisterGrabbable(RN::SceneNode* node);
		RN::Array* GetGrabbableObjects();

		void GenerateLevel();

		void TriggerSteveletWin(Stevelet* steve);

		RN::Model *AssignDefaultShader(RN::Model *model, bool transparent);
		
		bool IsPaused() const { return _isPaused; }
		bool IsDash() const { return _isDash; }

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
		RN::Array *_grabbableObjects;

		Helix *_helix;
		Player *_player;
		IPad* _ipad;
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
