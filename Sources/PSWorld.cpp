//
//  PSWorld.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSWorld.h"
#include "PSPlatformWrapper.h"

#if RN_PLATFORM_ANDROID
	#include "RNOculusMobileWindow.h"
#elif RN_PLATFORM_WINDOWS
	#ifdef BUILD_FOR_OCULUS
		#include "RNOculusWindow.h"
	#else
		#include "RNOculusWindow.h"
		#include "RNOpenVRWindow.h"
	#endif
#else
	#include "RNOpenVRWindow.h"
#endif

namespace PS
{
	World *World::_sharedInstance = nullptr;

	World *World::GetSharedInstance()
	{
		return _sharedInstance;
	}

	void World::Exit()
	{
		if(_sharedInstance)
		{
        	_sharedInstance->_audioWorld->Release();
        	_sharedInstance->RemoveAttachment(_sharedInstance->_audioWorld);

#if RN_PLATFORM_ANDROID
			if(_sharedInstance->_vrWindow)
			{
				_sharedInstance->_vrWindow->Release(); //Reference from VRCamera
				_sharedInstance->_vrWindow->Release(); //Reference from World
				_sharedInstance->_vrWindow->Release(); //Reference from Application
				//_sharedInstance->_vrWindow->StopRendering();
			}
#endif
		}

		Platform::Shutdown();
		exit(0);
		//RN::Kernel::GetSharedInstance()->Exit();
	}

	World::World(RN::VRWindow *vrWindow, RN::uint8 msaa, bool wantsPreview, bool debug) : _levelNodes(new RN::Array()), _shaderLibrary(nullptr), _vrWindow(nullptr), _msaa(msaa), _wantsPreviewWindow(wantsPreview), _debug(debug), _audioWorld(nullptr), _shadowCamera(nullptr), _vrCamera(nullptr), _previewWindow(nullptr), _copyEyeToScreenMaterial(nullptr), _isPaused(false), _isDash(false)
	{
		if(vrWindow)
			_vrWindow = vrWindow->Retain();
	}

	void World::WillBecomeActive()
	{
		Platform::Initialize();
		
		RN::Scene::WillBecomeActive();
		_shaderLibrary = RN::Renderer::GetActiveRenderer()->CreateShaderLibraryWithFile(RNCSTR("shaders/Shaders.json"));
		_sharedInstance = this;
		
		InitializePlatform();

		_physicsWorld = new RN::PhysXWorld(RN::Vector3(0.0, -9.81, 0.0), true);
		AddAttachment(_physicsWorld);
		
		_navigationWorld = new RN::RecastWorld();
		AddAttachment(_navigationWorld);

		if(_vrCamera)
		{
			_shadowCamera = new RN::Camera();
			_shadowCamera->SetFlags(_shadowCamera->GetFlags() | RN::Camera::Flags::NoRender);
			_vrCamera->GetHead()->AddChild(_shadowCamera->Autorelease());
			_vrCamera->SetClipFar(1500);
		}
		
/*		RN::OpenALSource *atmoAudioPlayer = _audioWorld->PlaySound(RN::AudioAsset::WithName(RNCSTR("audio/atmo.ogg")));
		atmoAudioPlayer->SetRepeat(true);*/
		
/*		_player = new Player(_mainCamera);
		AddNode(_player->Autorelease());
		_player->SetWorldPosition(RN::Vector3(0, 0, 0));*/
		
		LoadLevel();
	}

	void World::InitializePlatform()
	{
		RN::Vector2 resolution(960, 540);
		RN::Dictionary *resolutionDictionary = RN::Settings::GetSharedInstance()->GetEntryForKey<RN::Dictionary>(RNCSTR("RNResolution"));
		if(resolutionDictionary)
		{
			RN::Number *widthNumber = resolutionDictionary->GetObjectForKey<RN::Number>(RNCSTR("width"));
			RN::Number *heightNumber = resolutionDictionary->GetObjectForKey<RN::Number>(RNCSTR("height"));

			if (widthNumber)
			{
				resolution.x = widthNumber->GetFloatValue();
			}

			if (heightNumber)
			{
				resolution.y = heightNumber->GetFloatValue();
			}
		}

		if(_vrWindow)
		{
			RN::PostProcessingStage *monitorPass = nullptr;
			if(_wantsPreviewWindow)
			{
				RN::Window::SwapChainDescriptor swapchainDescriptor(RN::Texture::Format::BGRA_8_SRGB, (_msaa > 1) ? RN::Texture::Format::Invalid : RN::Texture::Format::Depth_24I);
				swapchainDescriptor.vsync = false;
				_previewWindow = RN::Renderer::GetActiveRenderer()->CreateAWindow(resolution, RN::Screen::GetMainScreen(), swapchainDescriptor);
				_previewWindow->Show();

				monitorPass = new RN::PostProcessingStage();
				_copyEyeToScreenMaterial = RN::Material::WithShaders(_shaderLibrary->GetShaderWithName(RNCSTR("pp_vertex")), _shaderLibrary->GetShaderWithName(RNCSTR("pp_blit_fragment")));
				monitorPass->SetFramebuffer(_previewWindow->GetFramebuffer());
				monitorPass->SetMaterial(_copyEyeToScreenMaterial);
				monitorPass->Autorelease();
			}

			RN::Window *debugWindow = nullptr;
			if(_debug)
			{
				debugWindow = RN::Renderer::GetActiveRenderer()->CreateAWindow(RN::Vector2(1920, 1080), RN::Screen::GetMainScreen());
			}

			_vrCamera = new RN::VRCamera(_vrWindow, monitorPass, _msaa, debugWindow);
			_vrCamera->GetEye(0)->GetRenderPass()->SetClearColor(RN::Color::Black());
			_vrCamera->GetEye(0)->GetRenderPass()->SetFlags(RN::RenderPass::Flags::ClearColor|RN::RenderPass::Flags::ClearDepthStencil);

#if RN_PLATFORM_ANDROID
			_vrCamera->GetEye(1)->GetRenderPass()->SetClearColor(RN::Color::Black());
			_vrCamera->GetEye(1)->GetRenderPass()->SetFlags(RN::RenderPass::Flags::ClearColor|RN::RenderPass::Flags::ClearDepthStencil);
#endif
			_mainCamera = _vrCamera;
		}
		else
		{
			RN::Window::SwapChainDescriptor swapchainDescriptor(RN::Texture::Format::BGRA_8_SRGB, (_msaa > 1) ? RN::Texture::Format::Invalid : RN::Texture::Format::Depth_24I);
			swapchainDescriptor.vsync = true;

#if (RN_PLATFORM_LINUX && !RN_BUILD_DEBUG)// || RN_PLATFORM_MAC_OS
//			swapchainDescriptor.wantsFullscreen = true;
#endif

			_previewWindow = RN::Renderer::GetActiveRenderer()->CreateAWindow(resolution, RN::Screen::GetMainScreen(), swapchainDescriptor);
			_previewWindow->Show();

#if (RN_PLATFORM_LINUX || RN_PLATFORM_WINDOWS) && !RN_BUILD_DEBUG
			_previewWindow->HideMouseCursor();

    #if RN_PLATFORM_WINDOWS || RN_PLATFORM_LINUX
			_previewWindow->TrapMouseCursor();
    #endif
#endif

			RN::Camera *camera = new RN::Camera();
			camera->SetFOV(60.0f);
			camera->GetRenderPass()->SetClearColor(RN::Color::Black());
			camera->GetRenderPass()->SetFlags(RN::RenderPass::Flags::ClearColor|RN::RenderPass::Flags::ClearDepthStencil);
			_mainCamera = camera;

			if(_msaa > 1)
			{
				RN::Texture *msaaTexture = RN::Texture::WithDescriptor(RN::Texture::Descriptor::With2DRenderTargetFormatAndMSAA(RN::Texture::Format::BGRA_8_SRGB, _previewWindow->GetSize().x, _previewWindow->GetSize().y, _msaa));
				RN::Texture *msaaDepthTexture = RN::Texture::WithDescriptor(RN::Texture::Descriptor::With2DRenderTargetFormatAndMSAA(RN::Texture::Format::Depth_24I, _previewWindow->GetSize().x, _previewWindow->GetSize().y, _msaa));
				RN::Framebuffer *msaaFramebuffer = RN::Renderer::GetActiveRenderer()->CreateFramebuffer(_previewWindow->GetSize());
				msaaFramebuffer->SetColorTarget(RN::Framebuffer::TargetView::WithTexture(msaaTexture));
				msaaFramebuffer->SetDepthStencilTarget(RN::Framebuffer::TargetView::WithTexture(msaaDepthTexture));

				camera->GetRenderPass()->SetFramebuffer(msaaFramebuffer);
				RN::PostProcessingAPIStage *resolvePass = new RN::PostProcessingAPIStage(RN::PostProcessingAPIStage::Type::ResolveMSAA);
				resolvePass->SetFramebuffer(_previewWindow->GetFramebuffer());
				camera->GetRenderPass()->AddRenderPass(resolvePass->Autorelease());

				RN::NotificationManager::GetSharedInstance()->AddSubscriber(kRNWindowDidChangeSize, [this, camera](RN::Notification *notification) {
					if(notification->GetName()->IsEqual(kRNWindowDidChangeSize))
					{
						RN::Texture *msaaTexture = RN::Texture::WithDescriptor(RN::Texture::Descriptor::With2DRenderTargetFormatAndMSAA(RN::Texture::Format::BGRA_8_SRGB, _previewWindow->GetSize().x, _previewWindow->GetSize().y, _msaa));
						RN::Texture *msaaDepthTexture = RN::Texture::WithDescriptor(RN::Texture::Descriptor::With2DRenderTargetFormatAndMSAA(RN::Texture::Format::Depth_24I, _previewWindow->GetSize().x, _previewWindow->GetSize().y, _msaa));
						RN::Framebuffer *msaaFramebuffer = RN::Renderer::GetActiveRenderer()->CreateFramebuffer(_previewWindow->GetSize());
						msaaFramebuffer->SetColorTarget(RN::Framebuffer::TargetView::WithTexture(msaaTexture));
						msaaFramebuffer->SetDepthStencilTarget(RN::Framebuffer::TargetView::WithTexture(msaaDepthTexture));

						camera->GetRenderPass()->SetFramebuffer(msaaFramebuffer);
						RN::PostProcessingAPIStage *resolvePass = new RN::PostProcessingAPIStage(RN::PostProcessingAPIStage::Type::ResolveMSAA);
						resolvePass->SetFramebuffer(_previewWindow->GetFramebuffer());
						camera->GetRenderPass()->AddRenderPass(resolvePass->Autorelease());
					}
				}, this);
			}
		}

		RN::NotificationManager::GetSharedInstance()->AddSubscriber(kRNWindowWillDestroy, [](RN::Notification *notification) {
			if (notification->GetName()->IsEqual(kRNWindowWillDestroy))
			{
				World::GetSharedInstance()->Exit();
			}
		}, this);

		RN::String *audioDevice = nullptr;
#if RN_PLATFORM_WINDOWS
		if(_vrWindow && _vrWindow->IsKindOfClass(RN::OculusWindow::GetMetaClass()))
		{
			RN::Array *audioDevices = RN::OpenALWorld::GetDeviceNames();
			audioDevices->Enumerate<RN::String>([&audioDevice](RN::String *string, size_t index, bool &stop) {
				if (string->GetRangeOfString(RNCSTR("Rift Audio")).length > 0)
				{
					audioDevice = string;
				}
			});
		}
#endif

		_audioWorld = new RN::OpenALWorld(audioDevice);
        AddAttachment(_audioWorld);

        if(_vrCamera)
        {
        	RN::OpenALListener *listener = new RN::OpenALListener();
        	_vrCamera->GetHead()->AddAttachment(listener);
        	_audioWorld->SetListener(listener);
        }
		else
		{
			RN::OpenALListener *listener = new RN::OpenALListener();
			_mainCamera->AddAttachment(listener);
			_audioWorld->SetListener(listener);
		}
	}
	
	void World::AddLevelNode(RN::SceneNode *node)
	{
		AddNode(node);
		_levelNodes->AddObject(node);
	}
	
	void World::RemoveLevelNode(RN::SceneNode *node)
	{
		RemoveNode(node);
		_levelNodes->RemoveObject(node);
	}
	
	void World::ClearLevel()
	{
		_levelNodes->Enumerate<RN::SceneNode>([&](RN::SceneNode *node, size_t index, bool &stop){
			RemoveNode(node);
		});
		_levelNodes->RemoveAllObjects();
		
		_navigationWorld->SetRecastMesh(nullptr, 10);
		
		FlushDeletionQueue();
	}

	void World::LoadLevel()
	{
/*		RN::Model *levelModel = RN::Model::WithName(RNCSTR("models/Wand_Boden_Raum.sgm"));
		_levelEntity = new RN::Entity(levelModel);
		AddNode(_levelEntity->Autorelease());
		
		RN::PhysXMaterial *levelPhysicsMaterial = new RN::PhysXMaterial();
		RN::PhysXCompoundShape *levelShape = RN::PhysXCompoundShape::WithModel(levelModel, levelPhysicsMaterial->Autorelease(), true);
		RN::PhysXStaticBody *levelBody = RN::PhysXStaticBody::WithShape(levelShape);
		levelBody->SetCollisionFilter(World::CollisionType::Level, World::CollisionType::All);
		_levelEntity->AddAttachment(levelBody);
		
		RN::Entity *skyEntity = new RN::Entity(RN::Model::WithSkydome(RNCSTR("models/sky_lightblue.*")));
		skyEntity->SetScale(RN::Vector3(1000.0f));
		skyEntity->SetWorldRotation(RN::Vector3(200.0f, 0.0f, 0.0f));
		AddNode(skyEntity->Autorelease());*/
	}

	void World::UpdateForWindowSize() const
	{
		if(!_previewWindow || !_copyEyeToScreenMaterial)
			return;

		RN::Vector2 eyeResolution((_vrWindow->GetSize().x - _vrWindow->GetEyePadding()) / 2.0f, _vrWindow->GetSize().y);
		RN::Vector2 windowResolution(_previewWindow->GetSize());
		float eyeAspect = eyeResolution.x / eyeResolution.y;
		float windowAspect = windowResolution.x / windowResolution.y;
		RN::Vector4 sourceRectangle = 0.0f;

		if(eyeAspect < windowAspect)
		{
			sourceRectangle.z = eyeResolution.x;
			sourceRectangle.w = eyeResolution.x / windowAspect;
			sourceRectangle.x = 0.0f;
			sourceRectangle.y = (eyeResolution.y - sourceRectangle.w) * 0.5f;
		}
		else
		{
			sourceRectangle.z = eyeResolution.y * windowAspect;
			sourceRectangle.w = eyeResolution.y;
			sourceRectangle.x = (eyeResolution.x - sourceRectangle.z) * 0.5f;
			sourceRectangle.y = 0.0f;
		}

		_copyEyeToScreenMaterial->SetDiffuseColor(RN::Color(sourceRectangle.x / _vrWindow->GetSize().x, sourceRectangle.y / _vrWindow->GetSize().y, (sourceRectangle.z / _vrWindow->GetSize().x), (sourceRectangle.w / _vrWindow->GetSize().y)));
	}

	void World::WillUpdate(float delta)
	{
		UpdateForWindowSize();
		
		RN::Camera *camera = _mainCamera->Downcast<RN::Camera>();
		float brightness = 1.0f;
		if(camera)
		{
			camera->SetAmbientColor(RN::Color::WithRGBA(brightness, brightness, brightness));
		}
		else
		{
			_vrCamera->GetEye(0)->SetAmbientColor(RN::Color::WithRGBA(brightness, brightness, brightness));
			_vrCamera->GetEye(1)->SetAmbientColor(RN::Color::WithRGBA(brightness, brightness, brightness));
		}
		
		RN::Scene::WillUpdate(delta);
	}

	void World::DidUpdate(float delta)
	{
		RN::Scene::DidUpdate(delta);

		Platform::HandleEvents();

		_isPaused = false;
		_isDash = false;
		if(_vrCamera)
		{
			if(_vrCamera->GetHMDTrackingState().mode == RN::VRHMDTrackingState::Mode::Paused)
			{
				_isPaused = true;
				_isDash = true;
			}
			else if(_vrCamera->GetHMDTrackingState().mode == RN::VRHMDTrackingState::Mode::Disconnected)
			{
				Exit();
			}
		}

		if(RN::InputManager::GetSharedInstance()->IsControlToggling(RNCSTR("ESC")))// || (_vrCamera && _vrCamera->GetControllerTrackingState(0).button[RN::VRControllerTrackingState::Button::Start]) || (_vrCamera && _vrCamera->GetControllerTrackingState(1).button[RN::VRControllerTrackingState::Button::Start]))
		{
			if(!_escWasPressed)
			{
				Exit();
			}
			_escWasPressed = true;
		}
		else
		{
			_escWasPressed = false;
		}

		_physicsWorld->SetPaused(_isPaused);
		_navigationWorld->SetPaused(_isPaused);
	}
}
