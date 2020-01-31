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

		RN::SceneNode *GetActiveHand() const { return _activeHand; }
		
		RN::Vector3 _handPosition;
		bool _wantsLevelPlacementReset;

	private:
		void CreateBody();
		void UpdateBody(const RN::Vector3 &direction, float delta);
		void UpdateArms();
		void PlaceEntityBetween(RN::Entity *entity, const RN::Vector3 &position1, const RN::Vector3 &position2);
		void SetAnimation(bool idle, bool leftHand, bool rightHand);
		
		RN::SceneNode *_camera;

		RN::Vector3 _previousHeadPosition;

		RN::Quaternion _cameraRotation;
		bool _stickIsPressed;
		bool _didSnapTurn;

		bool _didActivate;
		bool _isActivating;
		
		RN::Entity *_bodyEntity;
		RN::Entity *_handEntity[2];
		RN::Entity *_shoulderEntity[2];
		RN::Entity *_elbowEntity[2];
		RN::Entity *_upperArmEntity[2];
		RN::Entity *_lowerArmEntity[2];
		RN::SceneNode *_activeHand;
		
		bool _isIdle;
		int _lastActiveHand;
		
		RNDeclareMeta(Player)
	};
}

#endif /* defined(__ProjectSteve_PLAYER_H_) */
