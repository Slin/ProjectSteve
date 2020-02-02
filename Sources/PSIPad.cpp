//
//  LevelSelectorScreen.cpp
//  Project: Z
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSIPad.h"
#include "PSWorld.h"
#include "PSPlayer.h"
#include "PSSteveStats.h"

#define SCREEN_SIZE_X (0.16002f*2.0f-0.02f) //0.16002f
#define SCREEN_SIZE_Y (0.21336f*2.0f-0.0267f) //0.21336f
#define SCREEN_RESOLUTION_X 768 //1668
#define SCREEN_RESOLUTION_Y 1024 //2224
#define MARGIN_OUTSIDE 30
#define MARGIN_INSIDE 20
#define FONT_SIZE_BIG 50
#define FONT_SIZE_SMALL 30
#define LEVEL_DETAILS_IMAGE_SIZE 400
#define LEVEL_DETAILS_IMAGE_SPACING 70
#define LEVEL_DETAILS_LINE_SPACING 30

#if RN_PLATFORM_LINUX
#define MENU_FONT RNSTR("DejaVu Sans")
#else
#define MENU_FONT RNSTR("Helvetica")
#endif

namespace PS
{
	RNDefineMeta(IPad, Grabbable)

	IPad::IPad() : _needsRedraw(true), _isAnimating(false), _wasActivating(false), _uiWindow(nullptr), _isCursorActive(false)
	{
		SetModel(RN::Model::WithName(RNCSTR("models/tablet.sgm")));
		
		World *world = World::GetSharedInstance();
		RN::ShaderLibrary *shaderLibrary = world->GetShaderLibrary();
		
		RN::Mesh *mesh = RN::Mesh::WithTexturedCube(RN::Vector3(SCREEN_SIZE_X/2.0f, SCREEN_SIZE_Y/2.0f, 0.0f));
		RN::Shader::Options * shaderOptions = RN::Shader::Options::WithMesh(mesh);
		shaderOptions->AddDefine(RNCSTR("PZ_UNLIT"), RNCSTR("1"));
		_screenMaterial = RN::Material::WithShaders(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), shaderOptions), shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), shaderOptions));
		
		RN::Model *screenModel = new RN::Model();
		screenModel->AddLODStage(0.0f);
		screenModel->GetLODStage(0)->AddMesh(mesh, _screenMaterial);
		screenModel->CalculateBoundingVolumes();
		
		_screenEntity = new RN::Entity(screenModel->Autorelease());
		AddChild(_screenEntity->Autorelease());
		_screenEntity->SetPosition(RN::Vector3(0.0f, 0.0f, 0.005f));
		
		
		RN::Mesh *cursorMesh = RN::Mesh::WithColoredCube(RN::Vector3(0.002f, 0.002f, 0.002f), RN::Color::White());
		RN::Shader::Options * cursorShaderOptions = RN::Shader::Options::WithMesh(cursorMesh);
		cursorShaderOptions->AddDefine(RNCSTR("PZ_UNLIT"), RNCSTR("1"));
		RN::Material *cursorMaterial = RN::Material::WithShaders(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), cursorShaderOptions), shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), cursorShaderOptions));
		
		RN::Model *cursorModel = new RN::Model();
		cursorModel->AddLODStage(0.0f);
		cursorModel->GetLODStage(0)->AddMesh(cursorMesh, cursorMaterial);
		cursorModel->CalculateBoundingVolumes();
		
		_cursorEntity = new RN::Entity(cursorModel->Autorelease());
		AddChild(_cursorEntity->Autorelease());
		
		SetScale(RN::Vector3(0.75f, 0.75f, 0.75f));
		
		RN::PhysXMaterial *material = new RN::PhysXMaterial();
		RN::PhysXShape *shape = RN::PhysXBoxShape::WithHalfExtents(RN::Vector3(0.1f, 0.15f, 0.01f), material->Autorelease());
		_physicsBody = RN::PhysXDynamicBody::WithShape(shape, 0.1f);
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::All);
		AddAttachment(_physicsBody);
		
		
		//RN::AudioAsset *audioAssets = RN::AudioAsset::WithName(RNCSTR("audio/key_collect.ogg"));
		_audioSource = new RN::OpenALSource(nullptr);
		AddChild(_audioSource->Autorelease());
		_audioSource->SetPosition(RN::Vector3(0.0f, 0.0f, 0.0f));
		_audioSource->SetRepeat(false);
		
		CreateUI();
	}
	
	IPad::~IPad()
	{
		SafeRelease(_uiWindow);
	}
	
	void IPad::CreateUI()
	{
		World *world = World::GetSharedInstance();
		
		RN::Vector2 windowSize(SCREEN_RESOLUTION_X, SCREEN_RESOLUTION_Y);
		_uiWindow = new RN::UI::Window(RN::UI::Window::Style::Borderless, RN::Rect(0, 0, windowSize.x, windowSize.y), true);
		
		//Start level selection screen
		_levelScrollView = new RN::UI::ScrollView();
		_levelScrollView->SetPixelPerInch(SCREEN_SIZE_X/2.54f * SCREEN_RESOLUTION_X);
		_levelScrollView->SetFrame(RN::Rect(0.0f, 0.0f, windowSize.x, windowSize.y));
		
		
		_levelScrollView->SetBounds(_levelScrollView->GetFrame());
		_levelScrollView->SetBackgroundColor(RN::Color::ClearColor());
		
		float verticalPosition = MARGIN_OUTSIDE;
		
		RN::UI::Font *bigFont = RN::UI::Font::WithFamilyName(MENU_FONT, FONT_SIZE_BIG);
		RN::UI::Label *label = new RN::UI::Label();
		label->SetFrame(RN::Rect(MARGIN_OUTSIDE, verticalPosition, windowSize.x-MARGIN_OUTSIDE*2, FONT_SIZE_BIG));
		label->SetAlignment(RN::UI::Label::Alignment::Center);
		label->SetFont(bigFont);
		label->SetColor(RN::Color::White());
		label->SetBackgroundColor(RN::Color::ClearColor());
		label->SetText(RNCSTR("Known DNA Properties"));
		_levelScrollView->AddSubview(label->Autorelease());
		
		verticalPosition += FONT_SIZE_BIG + MARGIN_INSIDE * 2.0f;
		
		_attributesView = new RN::UI::View();
		_attributesView->SetFrame(RN::Rect(0.0f, verticalPosition, windowSize.x, windowSize.y));
		_levelScrollView->AddSubview(_attributesView->Autorelease());
		/*
		 STRENGTH,
		 SIZE,
		 INTELLIGENCE,
		 FAT_CONTENT,
		 AGGRO,
		 DEXTERITY,
		 SPEED,
		 WEIGHT,
		 WINGS,
		 SHELL,
		 HANDS,
		 LEGS,
		 LACTRASE,

		 FLYING,
		 CLIMB,
		 BOUNCE,
		 */
		
		_uiWindow->GetContentView()->SetBackgroundColor(RN::Color::Black());
		_uiWindow->GetContentView()->AddSubview(_levelScrollView->Autorelease());
		
		
		_uiWindow->Update();
		RN::Array *textureArray = new RN::Array();
		textureArray->AddObject(_uiWindow->GetBackingStore()->GetTexture());
		_screenMaterial->SetTextures(textureArray->Autorelease());
	}
	
	void IPad::UpdateCursorPosition()
	{
		if(_isCursorActive)
		{
			_needsRedraw = true;
		}
		
		_isCursorActive = false;
		_cursorEntity->AddFlags(RN::SceneNode::Flags::Hidden);
		
		RN::SceneNode *camera = World::GetSharedInstance()->GetCamera();
		RN::VRCamera *vrCamera = camera->Downcast<RN::VRCamera>();
		
		RN::Vector3 rayPosition;
		RN::Vector3 rayDirection;
		
		if(!vrCamera)
		{
			rayPosition = camera->GetWorldPosition();
			rayDirection = camera->GetForward();
		}
		else
		{
			Player *player = World::GetSharedInstance()->GetPlayer();
			RN::SceneNode *hand = player->GetHand(player->GetActiveHandIndex());
			if(hand)
			{
				rayPosition = hand->GetWorldPosition();
				rayDirection = hand->GetForward();
			}
		}
		
		if(rayPosition.GetDistance(GetWorldPosition()) > 1.0f) return;
		
		RN::Plane plane = RN::Plane::WithPositionNormal(GetWorldPosition(), GetForward());
		RN::Plane::ContactInfo contact = plane.CastRay(rayPosition, rayDirection);
		
		if(contact.distance < -0.5f) return;
		
		RN::Vector3 localPosition = contact.position - GetWorldPosition();
		localPosition = GetWorldRotation().GetConjugated().GetRotatedVector(localPosition);
		if(std::abs(localPosition.x) > SCREEN_SIZE_X/2.0f || std::abs(localPosition.y) > SCREEN_SIZE_Y/2.0f) return;
		
		_cursorEntity->SetPosition(localPosition);
		_cursorPosition = RN::Vector2(localPosition.x, localPosition.y);
		_cursorPosition += RN::Vector2(SCREEN_SIZE_X/2.0f, SCREEN_SIZE_Y/2.0f);
		_cursorPosition /= RN::Vector2(SCREEN_SIZE_X, SCREEN_SIZE_Y);
		_cursorPosition.y = 1.0f-_cursorPosition.y;
		_cursorPosition *= _uiWindow->GetContentSize();
		
		_cursorEntity->RemoveFlags(RN::SceneNode::Flags::Hidden);
		_isCursorActive = true;
	}
	
	void IPad::UpdateUI(float delta)
	{
		World *world = World::GetSharedInstance();
		Player *player = world->GetPlayer();
		if(!player) return;
		if(_isAnimating) return;
		
		_levelScrollView->Update(delta, _cursorPosition, player->IsActivatePressed() && _isCursorActive);

		
		
		_wasActivating = player->IsActivatePressed();
	}
	
	void IPad::Update(float delta)
	{
		if(World::GetSharedInstance()->IsPaused()) return;

		Grabbable::Update(delta);
		UpdateCursorPosition();
		
		_physicsBody->SetEnableKinematic(_isGrabbed);
		
		if(_wantsThrow)
		{
			_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
			_wantsThrow = false;
		}

		if(!_needsRedraw && !_isCursorActive && !_levelScrollView->IsScrolling() && !_isAnimating)
		{
			return;
		}

		_needsRedraw = false;

		UpdateUI(delta);

		_uiWindow->Update();
		_uiWindow->DrawViews();
	}

	void IPad::UpdateAttributes(const DNA& dna)
	{
		using namespace RN::UI;
		_attributesView->RemoveAllSubviews();
		Font* smallFont = RN::UI::Font::WithFamilyName(MENU_FONT, FONT_SIZE_SMALL);

		const SteveStats stats(dna);

		RN::Array* properties = RN::Array::WithObjects({ RNSTR("Strength"), RNSTR("Size"), RNSTR("Intelligence"), RNSTR("Bodyfat"), RNSTR("Aggression"), RNSTR("Dexterity"), RNSTR("Speed"), RNSTR("Weight"), RNSTR("Wings"), RNSTR("Shell"), RNSTR("Hands"), RNSTR("Legs"), RNSTR("Lactrase"), RNSTR("Flying"), RNSTR("Climbing"), RNSTR("Bouncing") });
		float verticalPosition = 0.f;
		properties->Enumerate<RN::String>([&](RN::String* property, size_t index, bool& stop)
		{
			const auto attr = static_cast<SteveStats::Attributes>(index);
			if (!StatsKnowledge::IsKnown(attr)) return;
			
			Label* label = new Label();
			label->SetFrame(RN::Rect(MARGIN_OUTSIDE, verticalPosition, SCREEN_RESOLUTION_X - MARGIN_OUTSIDE * 2, FONT_SIZE_SMALL));
			label->SetAlignment(RN::UI::Label::Alignment::Center);
			label->SetFont(smallFont);
			label->SetColor(RN::Color::White());
			label->SetBackgroundColor(RN::Color::ClearColor());
			label->SetText(RNSTR(property << ": " << stats[attr]));
			_attributesView->AddSubview(label->Autorelease());

			verticalPosition += MARGIN_INSIDE + FONT_SIZE_SMALL;
		});

		_needsRedraw = true;
	}
}
