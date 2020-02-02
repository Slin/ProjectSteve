//
//  PSWorld.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSWorld.h"
#include "PSPlatformWrapper.h"

#include "PSStevelet.h"
#include "PSPlayer.h"
#include "PSHelix.h"
#include "PSSyringe.h"
#include "PSLevel.h"
#include "PSObstacle.h"
#include "PSSpawner.h"
#include "PSIPad.h"

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

	World::World(RN::VRWindow *vrWindow, RN::uint8 msaa, bool wantsPreview, bool debug) : _levelNodes(new RN::Array()), _grabbableObjects(new RN::Array()), _shaderLibrary(nullptr), _vrWindow(nullptr), _msaa(msaa), _wantsPreviewWindow(wantsPreview), _debug(debug), _audioWorld(nullptr), _shadowCamera(nullptr), _vrCamera(nullptr), _previewWindow(nullptr), _copyEyeToScreenMaterial(nullptr), _isPaused(false), _isDash(false)
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
		
		_player = new Player(_mainCamera);
		AddNode(_player->Autorelease());
		_player->SetWorldPosition(RN::Vector3(0, 0, 0));
		
		LoadLevel();
	}

	void World::TriggerSteveletWin(Stevelet* steve) {
		// TODO: WIN
		RNDebug("Wow, you did it!");
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
			RN::Array *audioDevices = RN::OpenALWorld::GetOutputDeviceNames();
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
	
	void World::AddLevelNode(RN::SceneNode *node, bool _isGrabbable)
	{
		AddNode(node);
		_levelNodes->AddObject(node);
		if(_isGrabbable) _grabbableObjects->AddObject(node);
	}
	
	void World::RemoveLevelNode(RN::SceneNode *node)
	{
		RemoveNode(node);
		_levelNodes->RemoveObject(node);
		_grabbableObjects->RemoveObject(node);
	}
	
	void World::RegisterGrabbable(RN::SceneNode* node)
	{
		_grabbableObjects->AddObject(node);
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
		RN::Model *levelModel = AssignDefaultShader(RN::Model::WithName(RNCSTR("models/room.sgm")), false);
		RN::Entity *levelEntity = new RN::Entity(levelModel);
		AddLevelNode(levelEntity->Autorelease(), false);
		
		RN::PhysXMaterial *levelPhysicsMaterial = new RN::PhysXMaterial();
		levelPhysicsMaterial->Autorelease();
		RN::PhysXCompoundShape *levelShape = RN::PhysXCompoundShape::WithModel(levelModel, levelPhysicsMaterial, true);
		RN::PhysXStaticBody *levelBody = RN::PhysXStaticBody::WithShape(levelShape);
		levelBody->SetCollisionFilter(World::CollisionType::Level, World::CollisionType::All);
		levelEntity->AddAttachment(levelBody);
		
		RN::Model *tableModel = AssignDefaultShader(RN::Model::WithName(RNCSTR("models/table.sgm")), false);
		RN::Entity *tableEntity = new RN::Entity(tableModel);
		AddLevelNode(tableEntity->Autorelease(), false);
		
		RN::PhysXCompoundShape *tableShape = RN::PhysXCompoundShape::WithModel(tableModel, levelPhysicsMaterial, true);
		RN::PhysXStaticBody *tableBody = RN::PhysXStaticBody::WithShape(tableShape);
		tableBody->SetCollisionFilter(World::CollisionType::Level, World::CollisionType::All);
		tableEntity->AddAttachment(tableBody);
		
		RN::Model *detailsModel = AssignDefaultShader(RN::Model::WithName(RNCSTR("models/room_details.sgm")), false);
		RN::Entity *detailsEntity = new RN::Entity(detailsModel);
		AddLevelNode(detailsEntity->Autorelease(), false);
		
		
		Spawner *syringeSpawner = new Spawner(Spawner::ObjectType::SyringeType, 10);
		AddLevelNode(syringeSpawner->Autorelease(), false);
		syringeSpawner->SetWorldPosition(RN::Vector3(1.6 - (1.0f+0.8f), 0.82f, -1.75f));
		syringeSpawner->Rotate(RN::Vector3(90.0f, 0.0f, 0.0f));
		
		_ipad = new IPad();
		AddLevelNode(_ipad->Autorelease(), true);
		_ipad->SetWorldPosition(RN::Vector3(1.6 - (-0.875f+0.8f), 0.82f, -1.75f));
		_ipad->SetRotation(RN::Vector3(0.0f, -90.0f, 0.0f));
		
		_helix = new Helix(*this);
		AddLevelNode(_helix->Autorelease(), false);
		_helix->SetWorldPosition(RN::Vector3(1.6 - (0.625f+0.8f), 0.8f, -1.75f));
		
		Spawner *geneGreenSpawner = new Spawner(Spawner::ObjectType::GeneCType, 10);
		AddLevelNode(geneGreenSpawner->Autorelease(), false);
		geneGreenSpawner->SetWorldPosition(RN::Vector3(1.6 - (0.25f+0.8f), 0.82f, -1.75f));
		geneGreenSpawner->Rotate(RN::Vector3(90.0f, 0.0f, 0.0f));
		
		Spawner *geneBlueSpawner = new Spawner(Spawner::ObjectType::GeneGType, 10);
		AddLevelNode(geneBlueSpawner->Autorelease(), false);
		geneBlueSpawner->SetWorldPosition(RN::Vector3(1.6 - (0.0f+0.8f), 0.82f, -1.75f));
		geneBlueSpawner->Rotate(RN::Vector3(90.0f, 0.0f, 0.0f));
		
		Spawner *geneYellowSpawner = new Spawner(Spawner::ObjectType::GeneAType, 10);
		AddLevelNode(geneYellowSpawner->Autorelease(), false);
		geneYellowSpawner->SetWorldPosition(RN::Vector3(1.6 - (-0.25f+0.8f), 0.82f, -1.75f));
		geneYellowSpawner->Rotate(RN::Vector3(90.0f, 0.0f, 0.0f));
		
		Spawner *geneRedSpawner = new Spawner(Spawner::ObjectType::GeneTType, 10);
		AddLevelNode(geneRedSpawner->Autorelease(), false);
		geneRedSpawner->SetWorldPosition(RN::Vector3(1.6 - (-0.5f+0.8f), 0.82f, -1.75f));
		geneRedSpawner->Rotate(RN::Vector3(90.0f, 0.0f, 0.0f));
		
		Spawner *steveletSpawner = new Spawner(Spawner::ObjectType::SteveletType, 5);
		AddLevelNode(steveletSpawner->Autorelease(), false);
		steveletSpawner->SetWorldPosition(RN::Vector3(0.3f, 0.2f, -1.5f));

		GenerateLevel();
	}

	void World::GenerateLevel()
	{
		std::vector<Obstacle*> required;

		auto level = new PS::Level(true);
		AddLevelNode(level->Autorelease(), false);
		level->SetWorldPosition(RN::Vector3(-1.75f, 0.0f, -1.75f));

		auto obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		level->AddObstacle(obs);
		AddLevelNode(obs->Autorelease(), false);

		obs = new PS::Obstacle(RNCSTR("models/obstacle_pit.sgm"), nullptr, level);
		obs->SetEffect(new PitEffect());
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);

		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		for (float x = -0.1; x < 0.2f; x += 0.2f)
		{
			for (float z = -0.1f; z < 0.2f; z += 0.2f)
			{
				Animatable *flame = new Animatable(RNCSTR("sprites/Fire.png"));
				obs->AddChild(flame->Autorelease());
				flame->SetPosition(RN::Vector3(x, 0.57f, z));
				flame->SetScale(RN::Vector3(0.5f, 0.5f, 0.5f));
			}
		}
		obs->SetEffect(new BurnEffect(1));
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		RN::OpenALSource *fireAudioSource = new RN::OpenALSource(RN::AudioAsset::WithName(RNCSTR("audio/fire.ogg")));
		fireAudioSource->SetGain(0.2f);
		fireAudioSource->SetRepeat(true);
		fireAudioSource->Seek(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 19.0f));
		fireAudioSource->SetPosition(RN::Vector3(0.0f, 0.5f, 0.0f));
		obs->AddChild(fireAudioSource->Autorelease());
		fireAudioSource->Play();

		obs = new PS::Obstacle(RNCSTR("models/obstacle_wall.sgm"), nullptr, level);
		obs->SetEffect(new WallEffect());
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		level->AddObstacle(obs);
		AddLevelNode(obs->Autorelease(), false);

		obs = new PS::Obstacle(RNCSTR("models/obstacle_web.sgm"), RNCSTR("models/obstacle_web_collision.sgm"), level);
		obs->SetEffect(new SlowEffect());
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		for (float x = -0.1; x < 0.2f; x += 0.2f)
		{
			for (float z = -0.1f; z < 0.2f; z += 0.2f)
			{
				Animatable *flame = new Animatable(RNCSTR("sprites/Fire.png"));
				obs->AddChild(flame->Autorelease());
				flame->SetPosition(RN::Vector3(x, 0.60f, z));
				flame->SetScale(RN::Vector3(0.75f, 0.75f, 0.75f));
			}
		}
		obs->SetEffect(new BurnEffect(2));
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		fireAudioSource = new RN::OpenALSource(RN::AudioAsset::WithName(RNCSTR("audio/fire.ogg")));
		fireAudioSource->SetGain(0.4f);
		fireAudioSource->SetRepeat(true);
		fireAudioSource->Seek(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 19.0f));
		fireAudioSource->SetPosition(RN::Vector3(0.0f, 0.5f, 0.0f));
		obs->AddChild(fireAudioSource->Autorelease());
		fireAudioSource->Play();
		
		
		level = new PS::Level(false);
		AddLevelNode(level->Autorelease(), false);
		level->SetWorldPosition(RN::Vector3(-1.75f, 0.0f, 1.75f));

		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		level->AddObstacle(obs);
		AddLevelNode(obs->Autorelease(), false);
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		Animatable *milk = new Animatable(RNCSTR("sprites/Milk.png"));
		milk->isAnimated = false;
		obs->SetEffect(new MilkEffect());
		milk->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->SetSpecularColor(RN::Color::WithRGBA(1.0f, 4.0f, 0.0f, 0.0f));
		obs->AddChild(milk->Autorelease());
		milk->SetPosition(RN::Vector3(0.0f, 0.64f, 0.0f));
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_windtunnel.sgm"), nullptr, level);
		obs->SetEffect(new WindEffect());
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		RN::OpenALSource *windAudioSource = new RN::OpenALSource(RN::AudioAsset::WithName(RNCSTR("audio/fire.ogg")));
		windAudioSource->SetGain(1.0f);
		windAudioSource->SetRepeat(true);
		windAudioSource->Seek(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 90.0f));
		windAudioSource->SetPosition(RN::Vector3(0.0f, 0.6f, 0.0f));
		obs->AddChild(windAudioSource->Autorelease());
		windAudioSource->Play();
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		level->AddObstacle(obs);
		AddLevelNode(obs->Autorelease(), false);
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		RN::Entity *iceBear = new RN::Entity(RN::Model::WithName(RNCSTR("models/cow.sgm")));
		obs->AddChild(iceBear->Autorelease());
		iceBear->SetPosition(RN::Vector3(-0.07f, 0.5f, 0.0f));
		iceBear->SetRotation(RN::Vector3(180.0f, 0.0f, 0.0f));
		obs->SetEffect(new FightEffect());
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);

		obs = new PS::Obstacle(RNCSTR("models/obstacle_web.sgm"), RNCSTR("models/obstacle_web_collision.sgm"), level);
		obs->SetEffect(new SlowEffect());
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		for (float x = -0.1; x < 0.2f; x += 0.2f)
		{
			for (float z = -0.1f; z < 0.2f; z += 0.2f)
			{
				Animatable *flame = new Animatable(RNCSTR("sprites/Fire.png"));
				obs->AddChild(flame->Autorelease());
				flame->SetPosition(RN::Vector3(x, 0.64f, z));
			}
		}
		obs->SetEffect(new BurnEffect(3));
		level->AddObstacle(obs);
		required.push_back(obs);
		AddLevelNode(obs->Autorelease(), false);
		fireAudioSource = new RN::OpenALSource(RN::AudioAsset::WithName(RNCSTR("audio/fire.ogg")));
		fireAudioSource->SetGain(0.6f);
		fireAudioSource->SetRepeat(true);
		fireAudioSource->Seek(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 19.0f));
		fireAudioSource->SetPosition(RN::Vector3(0.0f, 0.5f, 0.0f));
		obs->AddChild(fireAudioSource->Autorelease());
		fireAudioSource->Play();
 
		obs = new PS::Obstacle(RNCSTR("models/obstacle_empty.sgm"), nullptr, level);
		obs->SetEffect(new FinishEffect(required, obs));
		Animatable *flag = new Animatable(RNCSTR("sprites/Flag.png"));
		obs->AddChild(flag->Autorelease());
		flag->SetPosition(RN::Vector3(0.0f, 0.64f, 0.0f));
		level->AddObstacle(obs);
		AddLevelNode(obs->Autorelease(), false);
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

	Grabbable* World::GetClosestGrabbableObject(RN::Vector3 position)
	{
		Grabbable* closestObject = nullptr;
		float closestDistance = 1000.0f;
		
		_grabbableObjects->Enumerate<Grabbable>([&](Grabbable* node, size_t index, bool &stop){
			if (node->IsGrabbed()) return;

			float distance = node->GetWorldPosition().GetSquaredDistance(position);
			if(distance < closestDistance)
			{
				closestDistance = distance;
				closestObject = node;
			}
		});
		
		return closestObject;
	}

	RN::Array* World::GetGrabbableObjects() {
		return _grabbableObjects;
	}

	std::pair<Grabbable*, float> World::GetClosestGrabbableObject(RN::Vector2 coordinate)
	{
		Grabbable* closestObject = nullptr;
		float closestDistance = 1000.0f;
		const RN::Camera* camera = static_cast<RN::Camera*>(GetCamera());
		// GetInverseProjectionMatrix
		const RN::Matrix viewProjection = camera->GetProjectionMatrix()*camera->GetViewMatrix();
	
		_grabbableObjects->Enumerate<Grabbable>([&](Grabbable* node, size_t index, bool& stop) {
			if (node->IsGrabbed()) return;

			const RN::Vector4 posScreen = (viewProjection * RN::Vector4(node->GetWorldPosition(),1.f));
			if (posScreen.z < 0.f) return;
			const RN::Vector2 ndc = RN::Vector2(posScreen.x, posScreen.y) / posScreen.w;
			const float distance = ndc.GetSquaredDistance(coordinate);
			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestObject = node;
			}
			});
		return std::make_pair(closestObject, closestDistance);
	}

	RN::Model *World::AssignDefaultShader(RN::Model *model, bool transparent)
	{
		World *world = World::GetSharedInstance();
		RN::ShaderLibrary *shaderLibrary = world->GetShaderLibrary();
		
		RN::Model::LODStage *lodStage = model->GetLODStage(0);
		for(int i = 0; i < lodStage->GetCount(); i++)
		{
			RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(lodStage->GetMeshAtIndex(i));
			
			RN::Material *material = lodStage->GetMaterialAtIndex(i);
			material->SetAlphaToCoverage(transparent);
			material->SetAmbientColor(RN::Color::White());
			
			material->SetVertexShader(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), shaderOptions));
			material->SetFragmentShader(shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), shaderOptions));
		}
		
		return model;
	}
}
