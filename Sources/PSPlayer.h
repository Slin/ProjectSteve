//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_PLAYER_H_
#define __ProjectSteve_PLAYER_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace PS
{
	class World;
	class Player : public RN::SceneNode
	{
	public:
		Player(RN::SceneNode *camera);
		~Player();
		
		void Update(float delta) override;
		
		bool IsActivatePressed();
		void DidActivate();

		RN::uint8 GetActiveHandIndex() const { return _lastActiveHand; }
		RN::SceneNode *GetHand(RN::uint8 handIndex) const { return _handEntity[handIndex]; }
		bool GetIsGrabbing(RN::uint8 handIndex) const { return _isHandGrabbing[handIndex]; }

	private:
		RN::SceneNode* FindGrabbable(bool vrMode, RN::uint8 handIndex);
		RN::SceneNode* Grab(RN::SceneNode* node, RN::uint8 handIndex);
		void ReleaseGrabbable(bool vrMode, RN::uint8 handIndex);

		RN::SceneNode *_camera;

		RN::Vector3 _previousHeadPosition;

		RN::Quaternion _cameraRotation;
		bool _stickIsPressed;
		bool _didSnapTurn;

		bool _didActivate;
		bool _isActivating;
		
		RN::Entity *_handEntity[2];
		bool _isHandGrabbing[2];
		RN::SceneNode *_grabbedObject[2];
		RN::Vector3 _grabbedObjectOffset[2];
		RN::Quaternion _grabbedObjectRotationOffset[2];
		
		RN::uint8 _lastActiveHand;
		
		RNDeclareMeta(Player)
	};
}

#endif /* defined(__ProjectSteve_PLAYER_H_) */
