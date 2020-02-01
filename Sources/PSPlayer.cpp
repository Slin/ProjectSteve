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
	
	Player::Player(RN::SceneNode *camera) : _camera(camera->Retain()), _stickIsPressed(false), _didSnapTurn(false), _isActivating(false), _didActivate(false), _lastActiveHand(0), _activeHand(nullptr)
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
		
		World *world = World::GetSharedInstance();
		RN::ShaderLibrary *shaderLibrary = world->GetShaderLibrary();
		
		RN::Model *handModel = RN::Model::WithName(RNCSTR("models/hand.sgm"));
		handModel->GetLODStage(0)->GetMaterialAtIndex(0)->SetAlphaToCoverage(false);
		for(int i = 0; i < 2; i++)
		{
			_handEntity[i] = new RN::Entity(handModel);
			AddChild(_handEntity[i]->Autorelease());
			
			_handEntity[i]->SetWorldPosition(RN::Vector3(0.0f, 1.0f, 0.0f));
		}
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
		
		if(vrCamera)
		{
			RN::VRControllerTrackingState leftController = vrCamera->GetControllerTrackingState(0);
			RN::VRControllerTrackingState rightController = vrCamera->GetControllerTrackingState(1);
			
			leftController.position = vrCamera->GetWorldRotation().GetRotatedVector(leftController.position);
			leftController.rotation = vrCamera->GetWorldRotation() * leftController.rotation;
			rightController.position = vrCamera->GetWorldRotation().GetRotatedVector(rightController.position);
			rightController.rotation = vrCamera->GetWorldRotation() * rightController.rotation;
			
			//Hand placement
			_handEntity[0]->SetRotation(leftController.rotation * RN::Vector3(0.0f, -45.0f, 0.0f));
			_handEntity[0]->SetPosition(vrCamera->GetWorldPosition() - GetWorldPosition() + leftController.position - _handEntity[0]->GetRotation().GetRotatedVector(RN::Vector3(0.0f, 0.0f, 0.0f)));
			_handEntity[1]->SetRotation(rightController.rotation * RN::Vector3(0.0f, -45.0f, 0.0f));
			_handEntity[1]->SetPosition(vrCamera->GetWorldPosition() - GetWorldPosition() + rightController.position - _handEntity[1]->GetRotation().GetRotatedVector(RN::Vector3(-0.0f, 0.0f, -0.0f)));

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
			
			_lastActiveHand = activeHand;
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
}
