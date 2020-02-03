//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSPlayer.h"
#include "PSWorld.h"
#include "PSGrabbable.h"
#include "PSHelix.h"
#include "PSSyringe.h"

namespace PS
{
	RNDefineMeta(Player, RN::SceneNode)
	
	Player::Player(RN::SceneNode *camera) 
		: _camera(camera->Retain()), 
		_stickIsPressed(false), 
		_didSnapTurn(false), 
		_isActivating(false), 
		_didActivate(false), 
		_isHandGrabbing{},
		_grabbedObject{},
		_lastActiveHand(0)
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
		
		//World *world = World::GetSharedInstance();
		//RN::ShaderLibrary *shaderLibrary = world->GetShaderLibrary();
		
		RN::Model *handModel = RN::Model::WithName(RNCSTR("models/hand.sgm"));
		handModel->GetLODStage(0)->GetMaterialAtIndex(0)->SetAlphaToCoverage(false);
		for(int i = 0; i < 2; i++)
		{
			_handEntity[i] = new RN::Entity(handModel);
			AddChild(_handEntity[i]->Autorelease());
			
			_handEntity[i]->SetWorldPosition(RN::Vector3(0.0f, 1.0f, 0.0f));
		}
		
		_grabbedObject[0] = nullptr;
		_grabbedObject[1] = nullptr;
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
			_handEntity[0]->AddFlags(RN::Entity::Flags::Hidden);
			_handEntity[1]->AddFlags(RN::Entity::Flags::Hidden);
		}
		else
		{
			_handEntity[0]->RemoveFlags(RN::Entity::Flags::Hidden);
			_handEntity[1]->RemoveFlags(RN::Entity::Flags::Hidden);
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

		bool active = (int)manager->IsControlToggling(RNCSTR("E")) || manager->IsControlToggling(RNCSTR("Button 1"));
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
		
		RN::Vector3 stickTranslation;

		if(vrCamera)
		{
			RN::VRControllerTrackingState leftController = vrCamera->GetControllerTrackingState(0);
			RN::VRControllerTrackingState rightController = vrCamera->GetControllerTrackingState(1);

			bool isOneHanded = !leftController.active || !rightController.active;

			if(isOneHanded)
			{
				RN::VRControllerTrackingState activeController = leftController;
				if(!activeController.active)
					activeController = rightController;

				if(activeController.active)
				{
					if(activeController.type == RN::VRControllerTrackingState::Type::ThreeDegreesOfFreedom)
					{
						//Oculus Go
						if(activeController.button[RN::VRControllerTrackingState::Button::Pad])
						{
							if(!_stickIsPressed)
							{
								if(activeController.trackpad.x > 0.5f)
								{
									_camera->SetRotation(_cameraRotation - RN::Vector3(45.0f, 0.0f, 0.0f));
									_didSnapTurn = true;
								}
								else if(activeController.trackpad.x < -0.5f)
								{
									_camera->SetRotation(_cameraRotation + RN::Vector3(45.0f, 0.0f, 0.0f));
									_didSnapTurn = true;
								}
							}
							
							_stickIsPressed = true;
						}
						else
						{
							_stickIsPressed = false;
							_didSnapTurn = false;
						}
					}
					else
					{
						//PC VR / Quest
						if(activeController.thumbstick.GetLength() > 0.25f || activeController.button[RN::VRControllerTrackingState::Button::Pad])
						{
							if(!_stickIsPressed)
							{
								if(activeController.thumbstick.x > 0.25f || activeController.trackpad.x > 0.5f)
								{
									_camera->SetRotation(_cameraRotation - RN::Vector3(45.0f, 0.0f, 0.0f));
									_didSnapTurn = true;
								}
								else if(activeController.thumbstick.x < -0.25f || activeController.trackpad.x < -0.5f)
								{
									_camera->SetRotation(_cameraRotation + RN::Vector3(45.0f, 0.0f, 0.0f));
									_didSnapTurn = true;
								}
							}

							_stickIsPressed = true;
						}
						else
						{
							_stickIsPressed = false;
							_didSnapTurn = false;
						}
					}
				}
			}
			else //Two controllers
			{
				if(std::abs(rightController.thumbstick.x) > 0.25f || (rightController.button[RN::VRControllerTrackingState::Button::Pad] && std::abs(rightController.trackpad.x) > 0.25f))
				{
					if(!_didSnapTurn)
					{
						if(rightController.thumbstick.x > 0.25f || (rightController.button[RN::VRControllerTrackingState::Button::Pad] && rightController.trackpad.x > 0.25f))
						{
							_camera->SetRotation(_cameraRotation - RN::Vector3(45.0f, 0.0f, 0.0f));
							_didSnapTurn = true;
						}
						else if(rightController.thumbstick.x < -0.25f || (rightController.button[RN::VRControllerTrackingState::Button::Pad] && rightController.trackpad.x < -0.25f))
						{
							_camera->SetRotation(_cameraRotation + RN::Vector3(45.0f, 0.0f, 0.0f));
							_didSnapTurn = true;
						}
					}
				}
				else
				{
					_didSnapTurn = false;
				}
				
				RN::Vector3 controllerRotation = leftController.rotation.GetEulerAngle();
				controllerRotation.y = 0.0f;
				stickTranslation += (_cameraRotation * RN::Quaternion(controllerRotation)).GetRotatedVector(RN::Vector3(leftController.thumbstick.x, 0.0f, -leftController.thumbstick.y));
				stickTranslation.y = 0.0f;
				stickTranslation.Normalize(1.5f * delta);
			}
		}
		
		_camera->Rotate(rotation * delta * 15.0f);
		if(_camera->GetEulerAngle().y > 85.0f)
		{
			_camera->SetWorldRotation(RN::Vector3(_camera->GetEulerAngle().x, 85.0f, _camera->GetEulerAngle().z));
		}
		if(_camera->GetEulerAngle().y < -85.0f)
		{
			_camera->SetWorldRotation(RN::Vector3(_camera->GetEulerAngle().x, -85.0f, _camera->GetEulerAngle().z));
		}
		_cameraRotation = _camera->GetRotation();
		

		RN::Vector3 globalTranslation;
		if(vrCamera)
		{
			RN::Vector3 localMovement = vrCamera->GetHead()->GetPosition() - _previousHeadPosition;
			_previousHeadPosition = vrCamera->GetHead()->GetPosition();
			localMovement.y = 0.0f;

			globalTranslation += _camera->GetWorldRotation().GetRotatedVector(localMovement) + stickTranslation;
		}
		else
		{
			RN::Vector3 translation{};
			if(manager->IsControlToggling(RNSTR("V")))
				translation -= RN::Vector3(0.f,0.f, delta);
			if (manager->IsControlToggling(RNSTR("I")))
				translation += RN::Vector3(0.f, 0.f, delta);
			if (manager->IsControlToggling(RNSTR("U")))
				translation -= RN::Vector3(delta, 0.f, 0.f);
			if (manager->IsControlToggling(RNSTR("A")))
				translation += RN::Vector3(delta, 0.f, 0.f);

			globalTranslation = _camera->GetWorldRotation().GetRotatedVector(translation);
		}

		Translate(globalTranslation);
		
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
		
		if(vrCamera)
		{
			int activeHand = _lastActiveHand;
			for(int i = 0; i < 2; i++)
			{
				RN::VRControllerTrackingState controller = vrCamera->GetControllerTrackingState(i);
				controller.position = vrCamera->GetWorldRotation().GetRotatedVector(controller.position);
				controller.rotation = vrCamera->GetWorldRotation() * controller.rotation;
				
				_handEntity[i]->SetRotation(controller.rotation * RN::Vector3(0.0f, -45.0f, 0.0f));
				_handEntity[i]->SetPosition(vrCamera->GetWorldPosition() - GetWorldPosition() + controller.position - _handEntity[i]->GetRotation().GetRotatedVector(RN::Vector3(0.0f, 0.0f, 0.0f)));
				
				if(controller.active && controller.handTrigger > 0.1f)
				{
					if(!_isHandGrabbing[i])
					{
						Grabbable* grabbedObject;
						if((grabbedObject = FindGrabbable(true, i, false)) && (grabbedObject = Grab(grabbedObject,i)))
						{
							_grabbedObjectOffset[i] = _handEntity[i]->GetWorldRotation().GetConjugated().GetRotatedVector(grabbedObject->GetWorldPosition() - _handEntity[i]->GetWorldPosition());
							_grabbedObjectRotationOffset[i] = _handEntity[i]->GetWorldRotation().GetConjugated() * grabbedObject->GetWorldRotation();
						}
					}
					else
					{
						_grabbedObject[i]->SetWorldPosition(_handEntity[i]->GetWorldPosition() + _handEntity[i]->GetWorldRotation().GetRotatedVector(_grabbedObjectOffset[i]));
						_grabbedObject[i]->SetWorldRotation(_handEntity[i]->GetWorldRotation() * _grabbedObjectRotationOffset[i]);
					}
				}
				else
				{
					if(_grabbedObject[i])
					{
						ReleaseGrabbable(true, i);
					}
				}
				
				if(_grabbedObject[i])
				{
					Grabbable *grabbable = _grabbedObject[i]->Downcast<Grabbable>();
					if(grabbable)
					{
						grabbable->SetIsTriggered((controller.active && controller.indexTrigger > 0.1f));
					}
				}
				
				if(controller.active && controller.indexTrigger > 0.1f)
				{
					activeHand = i;
				}
			}
			
			_lastActiveHand = activeHand;
		}
		else // emulate grabbing with mouse
		{
			using namespace RN;

			auto processHand = [&](int i, const char* key)
			{
				if (manager->IsControlToggling(RNSTR(key)))
				{
					if (!_isHandGrabbing[i])
					{
						Grabbable* grabbedObject;
						if ((grabbedObject = FindGrabbable(false, i, false)) && (grabbedObject = Grab(grabbedObject, i)))
						{
							_grabbedObjectOffset[i] = {};
							_grabbedObjectOffset[i].z = -std::min(1.5f, grabbedObject->GetWorldPosition().GetDistance(_camera->GetWorldPosition()));
						}
					}
					else
					{
						const Vector3 dir = _camera->GetWorldRotation().GetRotatedVector(_grabbedObjectOffset[i]);
						_grabbedObject[i]->SetWorldPosition(_camera->GetWorldPosition() + dir);

						_grabbedObject[i]->SetIsTriggered(manager->IsControlToggling(RNSTR("3")));
					}
				}
				else
				{
					if (_grabbedObject[i]) ReleaseGrabbable(false, i);
				}
			};
			processHand(0, "1");
			processHand(1, "2");

			if (_isHandGrabbing[0] && _isHandGrabbing[1])
			{
				const float minDist = std::max(_grabbedObjectOffset[0].z, _grabbedObjectOffset[1].z);
				_grabbedObjectOffset[0].z = minDist;
				_grabbedObjectOffset[1].z = minDist;
				const RN::Vector3 left = _camera->GetWorldRotation().GetRotatedVector(Vector3(0.1f, 0.f, 0.f));
				_grabbedObject[0]->Translate(-left);
				_grabbedObject[1]->Translate(left);
				const RN::Vector3 forward = _camera->GetWorldRotation().GetRotatedVector(Vector3(0.f, 0.f, 1.f));
				_grabbedObject[0]->SetWorldRotation(RN::Quaternion::WithLookAt(_grabbedObject[0]->IsKindOfClass(Syringe::GetMetaClass()) ? -forward : left));
				_grabbedObject[1]->SetWorldRotation(RN::Quaternion::WithLookAt(_grabbedObject[1]->IsKindOfClass(Syringe::GetMetaClass()) ? forward : -left));
			}
		}
	}
	
	bool Player::IsActivatePressed()
	{
		return _isActivating;
	}

	void Player::DidActivate()
	{
		_didActivate = true;
	}

	Grabbable* Player::FindGrabbable(bool vrMode, RN::uint8 handIndex, bool isDropping)
	{
		PS::World* world = World::GetSharedInstance();
		if (vrMode)
		{
			RN::Vector3 referencePosition = _handEntity[handIndex]->GetWorldPosition();
			if(isDropping && _grabbedObject[handIndex] && _grabbedObject[handIndex]->IsKindOfClass(Gene::GetMetaClass()))
			{
				referencePosition = _grabbedObject[handIndex]->GetWorldPosition();
			}
			
			Grabbable* grabbedObject = world->GetClosestGrabbableObject(referencePosition);
			if(grabbedObject && referencePosition.GetSquaredDistance(grabbedObject->GetWorldPosition()) < 0.06f)
				return grabbedObject;
		}
		else
		{
			// no C++17 :(
			auto res = world->GetClosestGrabbableObject(RN::Vector2(0.f, 0.f));
			Grabbable* grabbedObject = res.first;
			const float dist = res.second;
			if (dist < 0.1f)
				return grabbedObject;
		}
		return nullptr;
	}

	Grabbable* Player::Grab(Grabbable* node, RN::uint8 handIndex)
	{
		if (!node) return nullptr;

		if (node->IsKindOfClass(Gene::GetMetaClass()) && node->GetParent() && node->GetParent()->IsKindOfClass(Helix::GetMetaClass()))
		{
			Helix& helix = static_cast<Helix&>(*node->GetParent());
			node = helix.PickGene(static_cast<Gene&>(*node));
		}

		if (node)
		{
			_isHandGrabbing[handIndex] = true;
			_grabbedObject[handIndex] = node;
			static_cast<Grabbable*>(node)->SetIsGrabbed(true);
		}

		return node;
	}

	void Player::ReleaseGrabbable(bool vrMode, RN::uint8 handIndex)
	{
		assert(_isHandGrabbing[handIndex]);

		RN::SceneNode* node = _grabbedObject[handIndex];
		// drop gene in helix?
		if (node->IsKindOfClass(Gene::GetMetaClass()))
		{
			RN::SceneNode* target = FindGrabbable(vrMode, handIndex, true);
			if (target && target->IsKindOfClass(Gene::GetMetaClass())
				&& target->GetParent()
				&& target->GetParent()->IsKindOfClass(Helix::GetMetaClass()))
			{
				Helix& helix = static_cast<Helix&>(*target->GetParent());
				helix.PlaceGene(*static_cast<Gene*>(target), *static_cast<Gene*>(node));
			}
		}

		_isHandGrabbing[handIndex] = false;
		_grabbedObject[handIndex] = nullptr;
		static_cast<Grabbable*>(node)->SetIsGrabbed(false);
	}

	void Player::ReleaseObjectFromHandIfNeeded(Grabbable *grabbable)
	{
		for(int i = 0; i < 2; i++)
		{
			if(_grabbedObject[i] == grabbable)
			{
				_grabbedObject[i]->Downcast<Grabbable>()->SetIsGrabbed(false);
				_isHandGrabbing[i] = false;
				_grabbedObject[i] = nullptr;
			}
		}
	}
}
