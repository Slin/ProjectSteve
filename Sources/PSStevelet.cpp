//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSStevelet.h"
#include "PSWorld.h"
#include "PSSpawner.h"
#include "PSObstacle.h"
#include "PSSyringe.h"

namespace PS
{
	RNDefineMeta(Stevelet, Animatable)
	
Stevelet::Stevelet() : Animatable(RNCSTR("sprites/stevelet/0000000.png")), _isMoving(false), _isInObstacle(false), _flightTime(0.0f), _flightHeight(0.0f), _idleAudioTimer(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 20.0f)), _lastSyringe(nullptr)
	{
		RN::PhysXMaterial *material = new RN::PhysXMaterial();
		RN::PhysXShape *shape = RN::PhysXSphereShape::WithRadius(0.15, material);
		_physicsBody = RN::PhysXDynamicBody::WithShape(shape, 0.5f);
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::Level);
		AddAttachment(_physicsBody);
		
		_idleAudioAssets[0] = RN::AudioAsset::WithName(RNCSTR("audio/quietsch_1.ogg"))->Retain();
		_idleAudioAssets[1] = RN::AudioAsset::WithName(RNCSTR("audio/quietsch_2.ogg"))->Retain();
		_idleAudioAssets[2] = RN::AudioAsset::WithName(RNCSTR("audio/quietsch_3.ogg"))->Retain();
		_idleAudioAssets[3] = RN::AudioAsset::WithName(RNCSTR("audio/quietsch_4.ogg"))->Retain();
		_idleAudioAssets[4] = RN::AudioAsset::WithName(RNCSTR("audio/quietsch_5.ogg"))->Retain();
		
		_dieAudioAsset = RN::AudioAsset::WithName(RNCSTR("audio/die.ogg"))->Retain();
		_changeAudioAsset = RN::AudioAsset::WithName(RNCSTR("audio/transition.ogg"))->Retain();
		
		_audioSource = new RN::OpenALSource(nullptr);
		AddChild(_audioSource->Autorelease());
	}

	Stevelet::~Stevelet()
	{
		_idleAudioAssets[0]->Release();
		_idleAudioAssets[1]->Release();
		_idleAudioAssets[2]->Release();
		_idleAudioAssets[3]->Release();
		_idleAudioAssets[4]->Release();
		
		_dieAudioAsset->Release();
		_changeAudioAsset->Release();
	}

	void Stevelet::Kill() {
		_isMoving = false;
		_isInObstacle = false;
		ResetVelocity();

		_completedObstacles.clear();

		if (_isGrabbed) {
			LeaveObstacleCourse();
			return;
		}

		if(GetWorldPosition().z > -40.0f)
		{
			RN::OpenALSource *source = new RN::OpenALSource(_dieAudioAsset);
			World::GetSharedInstance()->AddNode(source->Autorelease());
			source->SetWorldPosition(GetWorldPosition());
			source->SetSelfdestruct(true);
			source->Seek(0.0f);
			source->Play();
		}
		
		RNDebug("kill");
		_spawner->ReturnToPool(this);
	}



	void Stevelet::Vanish() {
		
		RN::OpenALSource *source = new RN::OpenALSource(_dieAudioAsset);
		World::GetSharedInstance()->AddNode(source->Autorelease());
		source->SetWorldPosition(GetWorldPosition());
		source->SetSelfdestruct(true);
		source->Seek(0.0f);
		source->Play();
		
		SetWorldPosition({ 0, 0, -42 });
	}

	void Stevelet::Jump(float intensity, float velocity) {
		_physicsBody->ApplyImpulse({ 0, intensity*0.017f, 0 });
		SetVelocity(velocity);
	}

	void Stevelet::SetFlying(float height, float distance)
	{
		Jump(height);
		_flightTime = distance/_targetVelocity;
		_flightHeight = height;
	}
	
	void Stevelet::SetClimbing(float duration) {
		_climbTime = duration;
		
		_climbZ = GetForward().z > GetForward().x ? GetWorldPosition().z : GetWorldPosition().x;
	}

	void Stevelet::ResetVelocity() {
		_targetVelocity = _defaultVelocity;
	}

	void Stevelet::SetVelocity(float velocity) {
		_targetVelocity = velocity;
	}

	void Stevelet::EnterObstacleCourse(RN::Vector3 position)
	{
		_isInObstacle = true;
		_physicsBody->SetLinearVelocity(RN::Vector3());
		ResetVelocity();
		SetTargetPosition(position);
		
		RN::Vector3 direction = position - GetWorldPosition();
		SetTargetRotation(RN::Vector3(direction.x > 0.1f? 270:180.0f, 0.0f, 0.0f));
			//RN::Vector3(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z + 1.0f));
		
		RNDebug("Enter ObstacleBlubb");
	}

	void Stevelet::LeaveObstacleCourse()
	{
		_isInObstacle = false;
		float offset = RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(1.3f, 2.4f);
		
		if(GetForward().x > GetForward().z) SetTargetPosition(RN::Vector3(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z - offset));
		else SetTargetPosition(RN::Vector3(GetWorldPosition().x - offset, GetWorldPosition().y, GetWorldPosition().z));

		RNDebug("Leave ObstacleBlubb");
	}

	bool Stevelet::DidFinish(Obstacle* obs) {
		return _completedObstacles.find(obs) != _completedObstacles.end();
	}

	void Stevelet::Update(float delta)
	{
		delta = std::min(0.05f, delta);
		Animatable::Update(delta);
		
		if(_idleAudioTimer < 0.0f)
		{
			if(!_audioSource->IsPlaying() && !_isInObstacle)
			{
				_audioSource->SetAudioAsset(_idleAudioAssets[RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomInt32Range(0, 4)]);
				_audioSource->Seek(0.0f);
				_audioSource->Play();
			}
			_idleAudioTimer = RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 20.0f);
		}
		_idleAudioTimer -= delta;
		
		if(_flightTime > 0.0f)
		{
			_physicsBody->ApplyForce({0.0f, 9.81f*0.5f*0.013f, 0.0f});
		}
		_flightTime -= delta;

		if (_climbTime > 0.0f) {
			if(_physicsBody->GetLinearVelocity().y < 1.0f) _physicsBody->ApplyForce({ 0.0f, 13.0f * 0.5f * 0.017f, 0.0f });
			_climbTime -= delta;

			float newPos = GetForward().z > GetForward().x ? GetWorldPosition().z : GetWorldPosition().x;
			if ((newPos - _climbZ) > 0.1f) {
				_climbTime = 0.0f;
				_physicsBody->ClearForces();
			}
		}
		
		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		if(direction.GetLength() > 0.2f)
		{
			if(_isMoving)
			{
				RN::Vector3 linearVelocity = _physicsBody->GetLinearVelocity();
				linearVelocity.y = 0.0f;
				if(linearVelocity.GetLength() < _targetVelocity)
					_physicsBody->ApplyForce(direction.Normalize() * 0.08f);
			}
		}
		else
		{
			_isMoving = false;
		}
		
		if(!_isMoving)
		{
			SetTargetPosition(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomVector3Range(RN::Vector3(-1.3f, 0.15f, -1.7f), RN::Vector3(1.7f, 0.15f, 1.3f)));
		}
		
		if(_isGrabbed)
		{
			_physicsBody->SetLinearVelocity(RN::Vector3(0.0f, 9.81f*delta, 0.0f));
		}
			
		if(_wantsThrow)
		{
			_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
			_wantsThrow = false;
		}
		
		
		if(delta > RN::k::EpsilonFloat)
		{
			RN::Quaternion startRotation = GetWorldRotation();
			if(_targetRotation.GetDotProduct(startRotation) > 0.0f)
				startRotation = startRotation.GetConjugated();
			RN::Quaternion rotationSpeed = _targetRotation * startRotation;
			RN::Vector4 axisAngleSpeed = rotationSpeed.GetAxisAngle();
			if(axisAngleSpeed.w > 180.0f)
				axisAngleSpeed.w -= 360.0f;
			RN::Vector3 angularVelocity(axisAngleSpeed.x, axisAngleSpeed.y, axisAngleSpeed.z);
			angularVelocity *= axisAngleSpeed.w*M_PI;
			angularVelocity /= 180.0f;
			angularVelocity /= delta;
			const float l = angularVelocity.GetLength();
			if(!_isInObstacle && l > 60.f) angularVelocity *= 60.f / l;
			_physicsBody->SetAngularVelocity(angularVelocity * 0.1f);
		}
	}

	void Stevelet::SetTargetPosition(RN::Vector3 position)
	{
		_targetPosition = position;
		_isMoving = true;

		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		direction.Normalize();
		
		_targetRotation = RN::Quaternion::WithLookAt(-direction, RN::Vector3(0.0f, 1.0f, 0.0f), false);
	}

	void Stevelet::SetTargetRotation(RN::Quaternion rotation)
	{
		_targetRotation = rotation;
	}

	void Stevelet::SetSteveletStats(const SteveStats &stats, bool playSound, Syringe *syringe)
	{
		if(syringe && syringe == _lastSyringe) return;
		_lastSyringe = syringe;
		
		if(playSound)
		{
			_audioSource->SetAudioAsset(_changeAudioAsset);
			_audioSource->Seek(0.0f);
			_audioSource->Play();
		}
		
		_stats = stats;
		SetTexture(_stats.GetSteveletFileName());
	}

	SteveStats const& Stevelet::GetSteveletStats() {
		return _stats;
	}

	void Stevelet::SetTexture(RN::String *file)
	{
		const size_t hash = file->GetHash();
		if (hash == _textureHash) return;

		_textureHash = hash;
		RN::Material *material = _model->GetLODStage(0)->GetMaterialAtIndex(0);
		material->RemoveAllTextures();
		material->AddTexture(RN::Texture::WithName(file));
	}
}
