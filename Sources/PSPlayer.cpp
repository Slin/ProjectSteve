//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSPlayer.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Player, RN::SceneNode)
	
	Player::Player(RN::SceneNode *camera) : _camera(camera->Retain()), _stickIsPressed(false), _didSnapTurn(false), _bodyEntity(nullptr), _isActivating(false), _didActivate(false), _isIdle(false), _lastActiveHand(0), _activeHand(nullptr)
	{
		AddChild(camera);

		RN::VRCamera *vrCamera = _camera->Downcast<RN::VRCamera>();
		if(vrCamera)
		{
			_previousHeadPosition = vrCamera->GetHead()->GetPosition();
			if(vrCamera->GetOrigin() == RN::VRWindow::Origin::Head)
				_camera->SetPosition(RN::Vector3(0.0f, 1.8f, 0.0f));
			else
				_camera->SetPosition(RN::Vector3(0.0f, 0.0f, 0.0f));
		}
		else
		{
			_camera->SetPosition(RN::Vector3(0.0f, 1.8f, 0.0f));
		}
		
		CreateBody();
	}
	
	Player::~Player()
	{
		SafeRelease(_camera);
	}
	
	void Player::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		if(World::GetSharedInstance()->IsDash())
		{
			_bodyEntity->AddFlags(RN::Entity::Flags::Hidden);
		}
		else
		{
			_bodyEntity->RemoveFlags(RN::Entity::Flags::Hidden);
		}

		RN::InputManager *manager = RN::InputManager::GetSharedInstance();
		RN::VRCamera *vrCamera = _camera->Downcast<RN::VRCamera>();

		RN::Vector3 rotation(0.0);

		if(!vrCamera)
		{
			rotation.x = manager->GetMouseDelta().x;
			rotation.y = manager->GetMouseDelta().y;
			rotation = -rotation;
		}

		RN::Vector3 translation(0.0);
		bool active = (int)manager->IsControlToggling(RNCSTR("E"));
		if(vrCamera)
		{
			for(int i = 0; i < 2; i++)
			{
				RN::VRControllerTrackingState controllerState = vrCamera->GetControllerTrackingState(i);
				if(controllerState.active && controllerState.indexTrigger > 0.5f)
				{
					active = true;
				}
			}
		}

		if(!active) _didActivate = false;
		if(!_didActivate) _isActivating = active;
		else _isActivating = false;

		translation.x = ((int)manager->IsControlToggling(RNCSTR("D")) - (int)manager->IsControlToggling(RNCSTR("A")));
		translation.z = ((int)manager->IsControlToggling(RNCSTR("S")) - (int)manager->IsControlToggling(RNCSTR("W")));

		if(vrCamera)
		{
			RN::VRControllerTrackingState leftController = vrCamera->GetControllerTrackingState(0);
			RN::VRControllerTrackingState rightController = vrCamera->GetControllerTrackingState(1);
			
			_handPosition = rightController.position;
			_wantsLevelPlacementReset = leftController.button[RN::VRControllerTrackingState::Button::Start];
		}

		RN::Vector3 globalTranslaion;
		if(vrCamera)
		{
			RN::Vector3 localMovement = vrCamera->GetHead()->GetPosition() - _previousHeadPosition;
			_previousHeadPosition = vrCamera->GetHead()->GetPosition();
			localMovement.y = 0.0f;

			globalTranslaion += _camera->GetWorldRotation().GetRotatedVector(localMovement);
		}

		Translate(globalTranslaion);
		
		if(vrCamera)
		{
			RN::Vector3 localMovement = vrCamera->GetWorldPosition() - vrCamera->GetHead()->GetWorldPosition();

			if(vrCamera->GetOrigin() == RN::VRWindow::Origin::Head)
				localMovement.y = 1.8f;
			else
				localMovement.y = 0.0f;

			vrCamera->SetWorldPosition(localMovement + GetWorldPosition());
		}
		else
		{
			_camera->SetPosition(RN::Vector3(0.0f, 1.8f, 0.0f));
		}
		
		UpdateBody(translation, delta);
	}
	
	bool Player::IsActivatePressed()
	{
		return _isActivating;
	}

	void Player::DidActivate()
	{
		_didActivate = true;
	}
	
	void Player::CreateBody()
	{
		World *world = World::GetSharedInstance();
		RN::ShaderLibrary *shaderLibrary = world->GetShaderLibrary();
		
		//Create the body entity
		RN::Mesh *boxMesh = RN::Mesh::WithColoredCube(RN::Vector3(0.28f, 0.5f, 0.08f), RN::Color::White());
		RN::Shader::Options *boxShaderOptions = RN::Shader::Options::WithMesh(boxMesh);
		RN::Material *boxMaterial = RN::Material::WithShaders(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), boxShaderOptions), shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), boxShaderOptions));
		boxMaterial->SetDiffuseColor(RN::Color::WithRGBA(0.0f, 1.0f, 0.0f));
		
		RN::Model *playerModel = RN::Model::WithName(RNCSTR("models/player.sgm"));
		
		RN::Model::LODStage *lodStage = playerModel->GetLODStage(0);
		RN::Material *material = lodStage->GetMaterialAtIndex(0);
		RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(lodStage->GetMeshAtIndex(0));

		shaderOptions->AddDefine(RNCSTR("PZ_PLAYERFADE"), RNCSTR("1"));
		
		material->SetVertexShader(World::GetSharedInstance()->GetShaderLibrary()->GetShaderWithName(RNCSTR("main_vertex"), shaderOptions));
		material->SetFragmentShader(World::GetSharedInstance()->GetShaderLibrary()->GetShaderWithName(RNCSTR("main_fragment"), shaderOptions));

		material->SetBlendFactorSource(RN::BlendFactor::One, RN::BlendFactor::One);
		material->SetBlendFactorDestination(RN::BlendFactor::OneMinusSourceAlpha, RN::BlendFactor::OneMinusSourceAlpha);
		material->SetBlendOperation(RN::BlendOperation::Add, RN::BlendOperation::Add);
		material->SetAlphaToCoverage(false);
		material->SetSpecularColor(RN::Color::WithRGBA(-100.0f, 1.0f, 0.0f, 0.0f));
	
		_bodyEntity = new RN::Entity(playerModel);
		_bodyEntity->AddFlags(RN::Entity::Flags::DrawLate);
		AddChild(_bodyEntity->Autorelease());
		_bodyEntity->SetScale(RN::Vector3(0.01f, 0.01f, 0.01f));
		
		RN::Mesh *handMesh = RN::Mesh::WithColoredCube(RN::Vector3(0.05f, 0.05f, 0.05f), RN::Color::White());
		RN::Material *handMaterial = RN::Material::WithMaterial(boxMaterial);
		handMaterial->SetDiffuseColor(RN::Color::WithRGBA(1.0f, 0.878f, 0.768f));
		RN::Model *handModel = new RN::Model(handMesh, handMaterial);
		
		RN::Mesh *armMesh = RN::Mesh::WithColoredCube(RN::Vector3(0.03f, 0.03f, 0.5f), RN::Color::White());
		RN::Material *armMaterial = RN::Material::WithMaterial(boxMaterial);
		armMaterial->SetDiffuseColor(RN::Color::WithRGBA(0.0f, 0.0f, 1.0f));
		RN::Model *armModel = new RN::Model(armMesh, armMaterial);

		RN::Mesh *jointMesh = RN::Mesh::WithColoredCube(RN::Vector3(0.04f, 0.04f, 0.04f), RN::Color::White());
		RN::Material *jointMaterial = RN::Material::WithMaterial(boxMaterial);
		jointMaterial->SetDiffuseColor(RN::Color::WithRGBA(1.0f, 0.0f, 0.0f));
		RN::Model *jointModel = new RN::Model(jointMesh, jointMaterial);
		
		for(int i = 0; i < 2; i++)
		{
			_handEntity[i] = new RN::Entity(handModel);
			AddChild(_handEntity[i]->Autorelease());
			_handEntity[i]->AddFlags(RN::SceneNode::Flags::Hidden);
			
			_shoulderEntity[i] = new RN::Entity(jointModel);
			_shoulderEntity[i]->SetScale(1.5f);
			AddChild(_shoulderEntity[i]->Autorelease());
			_shoulderEntity[i]->AddFlags(RN::SceneNode::Flags::Hidden);
			
			_elbowEntity[i] = new RN::Entity(jointModel);
			AddChild(_elbowEntity[i]->Autorelease());
			_elbowEntity[i]->AddFlags(RN::SceneNode::Flags::Hidden);
			
			_upperArmEntity[i] = new RN::Entity(armModel);
			AddChild(_upperArmEntity[i]->Autorelease());
			_upperArmEntity[i]->AddFlags(RN::SceneNode::Flags::Hidden);
			
			_lowerArmEntity[i] = new RN::Entity(armModel);
			AddChild(_lowerArmEntity[i]->Autorelease());
			_lowerArmEntity[i]->AddFlags(RN::SceneNode::Flags::Hidden);
		}
	}

	void Player::SetAnimation(bool idle, bool leftHand, bool rightHand)
	{
		if(idle == _isIdle) return;
		_isIdle = idle;
		
		if(idle)
		{
			_bodyEntity->GetModel()->GetSkeleton()->SetAnimation(RNCSTR("idle"));
		}
		else
		{
			_bodyEntity->GetModel()->GetSkeleton()->SetAnimation(RNCSTR("walk"));
		}

		auto leftArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftArm"));
		auto leftForeArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftForeArm"));
		auto leftHandBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftHand"));
		leftArmBone.front()->absolute = false;
		leftForeArmBone.front()->absolute = false;
		leftHandBone.front()->absolute = false;

		auto rightArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightArm"));
		auto rightForeArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightForeArm"));
		auto rightHandBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightHand"));
		rightArmBone.front()->absolute = false;
		rightForeArmBone.front()->absolute = false;
		rightHandBone.front()->absolute = false;
		
		if(leftHand)
		{
			leftArmBone.front()->SetAnimation(nullptr);
			leftArmBone.front()->absolute = true;
			leftForeArmBone.front()->SetAnimation(nullptr);
			leftForeArmBone.front()->absolute = true;
			leftHandBone.front()->SetAnimation(nullptr);
			leftHandBone.front()->absolute = true;
		}
		
		if(rightHand)
		{
			rightArmBone.front()->SetAnimation(nullptr);
			rightArmBone.front()->absolute = true;
			rightForeArmBone.front()->SetAnimation(nullptr);
			rightForeArmBone.front()->absolute = true;
			rightHandBone.front()->SetAnimation(nullptr);
			rightHandBone.front()->absolute = true;
		}
	}
	
	void Player::UpdateBody(const RN::Vector3 &direction, float delta)
	{
		if(!_bodyEntity)
			return;
		
		//Calculate neck position
		RN::Vector3 eyeVector(0.0f, 0.35f, -0.08f);
		RN::Vector3 headPosition;
		RN::Vector3 neckPosition;
		RN::Quaternion headRotation;
		bool isHandAnimated[2] = {true, true};
		
		RN::VRCamera *vrCamera = _camera->Downcast<RN::VRCamera>();
		if(vrCamera)
		{
			headPosition.y = vrCamera->GetHead()->GetPosition().y + vrCamera->GetPosition().y;
			neckPosition = headPosition - vrCamera->GetHead()->GetWorldRotation().GetRotatedVector(eyeVector);
			
			headRotation = vrCamera->GetHead()->GetWorldRotation();
			
			RN::VRControllerTrackingState leftController = vrCamera->GetControllerTrackingState(0);
			RN::VRControllerTrackingState rightController = vrCamera->GetControllerTrackingState(1);
			
			leftController.position = vrCamera->GetWorldRotation().GetRotatedVector(leftController.position);
			leftController.rotation = vrCamera->GetWorldRotation() * leftController.rotation;
			rightController.position = vrCamera->GetWorldRotation().GetRotatedVector(rightController.position);
			rightController.rotation = vrCamera->GetWorldRotation() * rightController.rotation;
			
			isHandAnimated[0] = !leftController.active;
			isHandAnimated[1] = !rightController.active;
			
			//Hand placement
			_handEntity[0]->SetRotation(leftController.rotation * RN::Vector3(0.0f, -45.0f, 0.0f));
			_handEntity[0]->SetPosition(vrCamera->GetWorldPosition() - GetWorldPosition() + leftController.position - _handEntity[0]->GetRotation().GetRotatedVector(RN::Vector3(0.05f, 0.05f, -0.13f)));
			_handEntity[1]->SetRotation(rightController.rotation * RN::Vector3(0.0f, -45.0f, 0.0f));
			_handEntity[1]->SetPosition(vrCamera->GetWorldPosition() - GetWorldPosition() + rightController.position - _handEntity[1]->GetRotation().GetRotatedVector(RN::Vector3(-0.05f, 0.05f, -0.13f)));

			int activeHand = 0;
			if(leftController.active == rightController.active)
			{
				activeHand = 2; //TODO: Chose the controller with the last button press
				
				if(leftController.indexTrigger > 0.1f)
				{
					_activeHand = _handEntity[0];
				}
				else if(rightController.indexTrigger > 0.1f)
				{
					_activeHand = _handEntity[1];
				}
				else if(_activeHand == nullptr)
				{
					_activeHand = _handEntity[1];
				}
			}
			else
			{
				if(leftController.active)
				{
					activeHand = 0;
				}
				else
				{
					activeHand = 1;
				}
				_activeHand = _handEntity[activeHand];
			}
			
			if(activeHand != _lastActiveHand)
			{
				SetAnimation(true, leftController.active, rightController.active);
				SetAnimation(false, leftController.active, rightController.active);
			}
			_lastActiveHand = activeHand;
		}
		else
		{
			headRotation = _camera->GetWorldRotation();
			
			headPosition = _camera->GetWorldPosition() - GetWorldPosition();
			neckPosition = headPosition - _camera->GetWorldRotation().GetRotatedVector(eyeVector);
		}
		
		//Scale body from feet to head
		//float heightFeetToNeck = neckPosition.y;
		//_bodyEntity->SetScale(RN::Vector3(1.0f, heightFeetToNeck, 1.0f));
		
		
		RN::Model::LODStage *lodStage = _bodyEntity->GetModel()->GetLODStage(0);
		RN::Material *material = lodStage->GetMaterialAtIndex(0);
		material->SetSpecularColor(RN::Color::WithRGBA(-100.0f, 0.01f, 0.0f, 0.0f));

		RN::Vector3 bodyPosition = neckPosition;
		bodyPosition.y = 0.0f;
		if((neckPosition.y - 1.7) < -0.3)
		{
			bodyPosition.y = std::max(neckPosition.y - 1.4f, -1.0f);
			RN::Vector2 transparency = RN::Vector2(0.0f, 0.2f).GetLerp(RN::Vector2(70.0f, 0.02f), std::min(-bodyPosition.y * 2.0f, 1.0f));
			material->SetSpecularColor(RN::Color::WithRGBA(transparency.x, transparency.y, 0.0f, 0.0f));
		}
		_bodyEntity->SetPosition(bodyPosition);
		
		//Body rotation
		RN::Vector3 headBasedDirection = headRotation.GetRotatedVector(RN::Vector3(0.0f, 0.0f, -1.0f));
		headBasedDirection.y = 0.0f;
		headBasedDirection.Normalize();
		RN::Vector3 handBasedDirection;
		if(!isHandAnimated[0])
			handBasedDirection += _handEntity[0]->GetForward();
		if(!isHandAnimated[1])
			handBasedDirection += _handEntity[1]->GetForward();
		handBasedDirection.y = 0.0f;
		handBasedDirection.Normalize();

		float headHandAngle = handBasedDirection.GetDotProduct(headBasedDirection);
		float lerpFactor = 0.5;
		if(headHandAngle < 0.0f)
		{
			lerpFactor += headHandAngle;
			lerpFactor = std::max(lerpFactor, 0.0f);
		}

		RN::Vector3 bodyDirection = headBasedDirection.GetLerp(handBasedDirection, lerpFactor);
		RN::Quaternion bodyRotation = RN::Quaternion::WithLookAt(-bodyDirection);
		_bodyEntity->SetWorldRotation(bodyRotation);
		
		RN::Quaternion handRotation(bodyRotation);
		
		if(direction.GetLength() > 0.01)
		{
			SetAnimation(false, !isHandAnimated[0], !isHandAnimated[1]);
			float animationSpeed = direction.GetLength();
			if(direction.z > 0.0f)
			{
				animationSpeed *= -1.0f;
			}
			_bodyEntity->GetModel()->GetSkeleton()->Update(animationSpeed * delta * 24.0f * 1.25f);
		}
		else
		{
			SetAnimation(true, !isHandAnimated[0], !isHandAnimated[1]);
			_bodyEntity->GetModel()->GetSkeleton()->Update(delta * 24.0f);
		}
		
		auto leftShoulderBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftShoulder"));
		auto leftArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftArm"));
		RN::Vector3 leftShouldBoneOffset = leftShoulderBone.front()->invBaseMatrix * leftArmBone.front()->invBaseMatrix.GetInverse() * RN::Vector3();
		RN::Vector3 leftShoulderPositionSkeletonSpace = leftShoulderBone.front()->finalMatrix * leftShoulderBone.front()->invBaseMatrix.GetInverse() * leftShouldBoneOffset;
		RN::Vector3 leftShoulderPosition = _bodyEntity->GetPosition() + handRotation.GetRotatedVector(leftShoulderPositionSkeletonSpace) * 0.01f;
		
		auto rightShoulderBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightShoulder"));
		auto rightArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightArm"));
		RN::Vector3 rightShouldBoneOffset = rightShoulderBone.front()->invBaseMatrix * rightArmBone.front()->invBaseMatrix.GetInverse() * RN::Vector3();
		RN::Vector3 rightShoulderPositionSkeletonSpace = rightShoulderBone.front()->finalMatrix * rightShoulderBone.front()->invBaseMatrix.GetInverse() * rightShouldBoneOffset;
		RN::Vector3 rightShoulderPosition = _bodyEntity->GetPosition() + handRotation.GetRotatedVector(rightShoulderPositionSkeletonSpace) * 0.01f;
		
		_shoulderEntity[0]->SetPosition(leftShoulderPosition);
		_shoulderEntity[0]->SetRotation(handRotation);
		_shoulderEntity[1]->SetPosition(rightShoulderPosition);
		_shoulderEntity[1]->SetRotation(handRotation);
		
		UpdateArms();
		
		auto leftForeArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftForeArm"));
		auto rightForeArmBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightForeArm"));
		auto leftHandBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:LeftHand"));
		auto rightHandBone = _bodyEntity->GetModel()->GetSkeleton()->GetBones(RNCSTR("mixamorig:RightHand"));
		
		if(!isHandAnimated[0])
		{
			RN::Quaternion leftHandRotation = handRotation.GetConjugated() * _handEntity[0]->GetRotation() * RN::Quaternion(RN::Vector3(0.0f, 180.0f, 90.0f)) * RN::Quaternion(RN::Vector3(90.0f, 0.0f, 0.0f));
			
			RN::Vector3 leftArmDir = _elbowEntity[0]->GetPosition() - leftShoulderPosition;
			float leftArmLength = leftArmDir.GetLength();
			leftArmDir = handRotation.GetConjugated().GetRotatedVector(leftArmDir);
			leftArmDir.Normalize();
			RN::Vector3 leftArmUpVector = leftHandRotation.GetRotatedVector(RN::Vector3(0.0f, 1.0f, 0.0f));
			RN::Quaternion leftArmToRot = RN::Quaternion::WithLookAt(leftArmDir, leftArmUpVector);
			leftArmBone.front()->position = leftShoulderPositionSkeletonSpace;
			leftArmBone.front()->rotation = leftArmToRot * RN::Quaternion(RN::Vector3(90.0f, 0.0f, 0.0f));
			leftArmBone.front()->scale = RN::Vector3(leftArmLength / 0.257146f, 1.0f, 1.0f);
			
			RN::Vector3 leftForeArmDir = _handEntity[0]->GetPosition() - _elbowEntity[0]->GetPosition();
			float leftForeArmLength = leftForeArmDir.GetLength();
			leftForeArmDir = handRotation.GetConjugated().GetRotatedVector(leftForeArmDir);
			leftForeArmDir.Normalize();
			RN::Vector3 leftForeArmUpVector = leftHandRotation.GetRotatedVector(RN::Vector3(0.0f, 1.0f, 0.0f));
			RN::Quaternion leftForeArmToRot = RN::Quaternion::WithLookAt(leftForeArmDir, leftForeArmUpVector);
			leftForeArmBone.front()->position = handRotation.GetConjugated().GetRotatedVector(_elbowEntity[0]->GetPosition() - _bodyEntity->GetPosition()) * 100.0f;
			leftForeArmBone.front()->rotation = leftForeArmToRot * RN::Quaternion(RN::Vector3(90.0f, 0.0f, 0.0f));
			leftForeArmBone.front()->scale = RN::Vector3(leftForeArmLength / 0.249858, 1.0f, 1.0f);
			
			leftHandBone.front()->position = handRotation.GetConjugated().GetRotatedVector(_handEntity[0]->GetPosition() - _bodyEntity->GetPosition())*100.0f;
			leftHandBone.front()->rotation = leftHandRotation;
		}
		
		if(!isHandAnimated[1])
		{
			RN::Quaternion rightHandRotation = handRotation.GetConjugated() * _handEntity[1]->GetRotation() * RN::Quaternion(RN::Vector3(0.0f, 180.0f, -90.0f)) * RN::Quaternion(RN::Vector3(-90.0f, 0.0f, 0.0f));

			RN::Vector3 rightArmDir = _elbowEntity[1]->GetPosition() - rightShoulderPosition;
			float rightArmLength = rightArmDir.GetLength();
			rightArmDir = handRotation.GetConjugated().GetRotatedVector(rightArmDir);
			rightArmDir.Normalize();
			RN::Vector3 rightArmUpVector = rightHandRotation.GetRotatedVector(RN::Vector3(0.0f, 1.0f, 0.0f));
			RN::Quaternion rightArmToRot = RN::Quaternion::WithLookAt(rightArmDir, rightArmUpVector);
			rightArmBone.front()->position = rightShoulderPositionSkeletonSpace;
			rightArmBone.front()->rotation = rightArmToRot * RN::Quaternion(RN::Vector3(-90.0f, 0.0f, 0.0f));
			rightArmBone.front()->scale = RN::Vector3(rightArmLength / 0.257146f, 1.0f, 1.0f);
			
			RN::Vector3 rightForeArmDir = _handEntity[1]->GetPosition() - _elbowEntity[1]->GetPosition();
			float rightForeArmLength = rightForeArmDir.GetLength();
			rightForeArmDir = handRotation.GetConjugated().GetRotatedVector(rightForeArmDir);
			rightForeArmDir.Normalize();
			RN::Vector3 rightForeArmUpVector = rightHandRotation.GetRotatedVector(RN::Vector3(0.0f, 1.0f, 0.0f));
			RN::Quaternion rightForeArmToRot = RN::Quaternion::WithLookAt(rightForeArmDir, rightForeArmUpVector);
			rightForeArmBone.front()->position = handRotation.GetConjugated().GetRotatedVector(_elbowEntity[1]->GetPosition() - _bodyEntity->GetPosition()) * 100.0f;
			rightForeArmBone.front()->rotation = rightForeArmToRot * RN::Quaternion(RN::Vector3(-90.0f, 0.0f, 0.0f));
			rightForeArmBone.front()->scale = RN::Vector3(rightForeArmLength / 0.249858, 1.0f, 1.0f);
			
			rightHandBone.front()->position = handRotation.GetConjugated().GetRotatedVector(_handEntity[1]->GetPosition() - _bodyEntity->GetPosition()) * 100.0f;
			rightHandBone.front()->rotation = rightHandRotation;
		}
 
		_bodyEntity->GetModel()->GetSkeleton()->Update(0.0f);
		
		if(!isHandAnimated[0])
		{
			leftArmBone.front()->rotation = RN::Quaternion();
			leftForeArmBone.front()->rotation = RN::Quaternion();
			leftHandBone.front()->rotation = RN::Quaternion();
		}
		
		if(!isHandAnimated[1])
		{
			rightArmBone.front()->rotation = RN::Quaternion();
			rightForeArmBone.front()->rotation = RN::Quaternion();
			rightHandBone.front()->rotation = RN::Quaternion();
		}
	}
	
	void Player::PlaceEntityBetween(RN::Entity *entity, const RN::Vector3 &position1, const RN::Vector3 &position2)
	{
		RN::Vector3 difference = position2 - position1;
		entity->SetPosition(position1 + difference * 0.5f);
		entity->SetRotation(RN::Quaternion::WithLookAt(difference.GetNormalized()));
		entity->SetScale(RN::Vector3(1.0f, 1.0f, difference.GetLength()));
	}
	
	void Player::UpdateArms()
	{
		for(int i = 0; i < 2; i++)
		{
			RN::Vector3 shoulderToHand = _handEntity[i]->GetPosition() - _shoulderEntity[i]->GetPosition();
			float shoulderToHandLength = shoulderToHand.GetLength();
			
			float halfArmLength = std::max(shoulderToHandLength  + 0.001f, 0.65f) * 0.5f;
			
			RN::Vector3 elbowReference = _shoulderEntity[i]->GetPosition() + shoulderToHand*0.5f;
			RN::Vector3 elbowDirection = _handEntity[i]->GetRight().GetCrossProduct(shoulderToHand.GetNormalized());
			float elbowOffsetScale = std::sqrt(halfArmLength * halfArmLength - shoulderToHandLength * shoulderToHandLength * 0.25f);
			
			_elbowEntity[i]->SetRotation(_handEntity[i]->GetRotation());
			_elbowEntity[i]->SetPosition(elbowReference - elbowDirection * elbowOffsetScale);
			
			PlaceEntityBetween(_upperArmEntity[i], _shoulderEntity[i]->GetPosition(), _elbowEntity[i]->GetPosition());
			PlaceEntityBetween(_lowerArmEntity[i], _elbowEntity[i]->GetPosition(), _handEntity[i]->GetPosition());
		}
	}
}
