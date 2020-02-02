//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_STEVELET_H_
#define __ProjectSteve_STEVELET_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>
#include "PSAnimatable.h"
#include "PSSteveStats.h"
#include <set>

namespace PS
{
	class World;
	class Obstacle;
	class Syringe;
	class Stevelet : public Animatable
	{
	public:
		Stevelet();
		~Stevelet();
		void Update(float delta) override;

		void Kill();
		void ResetVelocity();
		void SetVelocity(float velocity);
		void Jump(float intensity, float velocity = 0.4f);
		void SetFlying(float height, float distance);
		void SetClimbing(float duration);
		void Vanish();
		void EnterObstacleCourse(RN::Vector3 position);
		void LeaveObstacleCourse();
		
		void SetTargetPosition(RN::Vector3 position);
		void SetTargetRotation(RN::Quaternion rotation);

		bool DidFinish(Obstacle* obs);
		
		void SetSteveletStats(const SteveStats &stats, bool playSound, Syringe *syringe);
		SteveStats const& GetSteveletStats();
		
	private:
		SteveStats _stats;
		RN::Vector3 _targetPosition;
		RN::Quaternion _targetRotation;
		bool _isMoving;
		bool _isInObstacle;
		
		Syringe *_lastSyringe;
		
		RN::AudioAsset *_idleAudioAssets[5];
		RN::AudioAsset *_grabAudioAssets[2];
		RN::AudioAsset *_dieAudioAsset;
		RN::AudioAsset *_changeAudioAsset;
		
		RN::OpenALSource *_audioSource;
		float _idleAudioTimer;

		std::set<Obstacle*> _completedObstacles;
		
		void SetTexture(RN::String *file);
		size_t _textureHash = 0;

		float const _defaultVelocity = 0.4f;
		float _targetVelocity = _defaultVelocity;
		
		float _flightTime;
		float _flightHeight;

		float _climbTime;
		float _climbZ;
		
		bool _wasGrabbed = false;
		
		RN::PhysXDynamicBody *_physicsBody;
		
		RNDeclareMeta(Stevelet)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
