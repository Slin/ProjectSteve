//
//  PSApplication.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSApplication.h"
#include "PSWorld.h"

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
	Application::Application() : _vrWindow(nullptr)
	{

	}

	Application::~Application()
	{
		SafeRelease(_vrWindow);
	}

	void Application::SetupVR()
	{
		if (_vrWindow)
			return;

#if RN_PLATFORM_ANDROID
		_vrWindow = new RN::OculusMobileWindow();
#else
	bool wantsOpenVR = RN::Kernel::GetSharedInstance()->GetArguments().HasArgument("openvr", 0);
	#if RN_PLATFORM_WINDOWS
		bool wantsOculus = RN::Kernel::GetSharedInstance()->GetArguments().HasArgument("oculusvr", 0);
		
		#ifndef BUILD_FOR_OCULUS
		if(!wantsOpenVR && (!RN::OpenVRWindow::IsSteamVRRunning() || wantsOculus))
		#endif
		{
			if(RN::OculusWindow::GetAvailability() == RN::VRWindow::HMD || wantsOculus)
			{
				_vrWindow = new RN::OculusWindow();
				return;
			}
		}
	#endif

	#ifndef BUILD_FOR_OCULUS
		if(RN::OpenVRWindow::GetAvailability() == RN::VRWindow::HMD || wantsOpenVR)
		{
			_vrWindow = new RN::OpenVRWindow();
			return;
		}
	#endif
#endif
	}

	RN::RendererDescriptor *Application::GetPreferredRenderer() const
	{
		if(!_vrWindow) return RN::Application::GetPreferredRenderer();

		RN::Array *instanceExtensions = _vrWindow->GetRequiredVulkanInstanceExtensions();

		RN::Dictionary *parameters = nullptr;
		
		if(instanceExtensions)
		{
			parameters = new RN::Dictionary();
			parameters->SetObjectForKey(instanceExtensions, RNCSTR("instanceextensions"));
		}

		RN::RendererDescriptor *descriptor = RN::RendererDescriptor::GetPreferredRenderer(parameters);
		return descriptor;
	}

	RN::RenderingDevice *Application::GetPreferredRenderingDevice(RN::RendererDescriptor *descriptor, const RN::Array *devices) const
	{
		if(!_vrWindow) return RN::Application::GetPreferredRenderingDevice(descriptor, devices);

		RN::RenderingDevice *preferred = nullptr;

		RN::RenderingDevice *vrDevice = _vrWindow->GetOutputDevice(descriptor);
		if(vrDevice)
		{
			devices->Enumerate<RN::RenderingDevice>([&](RN::RenderingDevice *device, size_t index, bool &stop) {
				if(vrDevice->IsEqual(device))
				{
					preferred = device;
					stop = true;
				}
			});
		}

		if(!preferred)
			preferred = RN::Application::GetPreferredRenderingDevice(descriptor, devices);

		RN::Array *deviceExtensions = _vrWindow->GetRequiredVulkanDeviceExtensions(descriptor, preferred);
		preferred->SetExtensions(deviceExtensions);

		return preferred;
	}

	void Application::WillFinishLaunching(RN::Kernel *kernel)
	{
		bool isPancake = false;
		if(RN::Kernel::GetSharedInstance()->GetArguments().HasArgument("pancake", '2d'))
		{
			isPancake = true;
		}

		if(!isPancake)
		{
			SetupVR();
		}

		RN::Application::WillFinishLaunching(kernel);

#if RN_PLATFORM_ANDROID
		RN::Shader::Sampler::SetDefaultAnisotropy(2);
#else
		RN::Shader::Sampler::SetDefaultAnisotropy(16);
#endif
	}

	void Application::DidFinishLaunching(RN::Kernel *kernel)
	{
		if(_vrWindow)
		{
			RN::Renderer *renderer = RN::Renderer::GetActiveRenderer();
			renderer->SetMainWindow(_vrWindow);

			RN::Window::SwapChainDescriptor swapChainDescriptor;
#if RN_PLATFORM_WINDOWS
			if(_vrWindow->IsKindOfClass(RN::OculusWindow::GetMetaClass()))
			{
				swapChainDescriptor.depthStencilFormat = RN::Texture::Format::Depth_32F;
			}
#endif
			_vrWindow->StartRendering(swapChainDescriptor);
		}

		RN::Application::DidFinishLaunching(kernel);
		
		bool wantsPreview = false;
		if(RN::Kernel::GetSharedInstance()->GetArguments().HasArgument("preview", 'p'))
		{
			wantsPreview = true;
		}

#if RN_PLATFORM_MAC_OS
		World *world = new World(_vrWindow, 0, wantsPreview, false);
#elif RN_PLATFORM_ANDROID
		World *world = new World(_vrWindow, 4, false, (_vrWindow == nullptr));
#elif RN_PLATFORM_LINUX
        World *world = new World(_vrWindow, 8, false, false);
#else
		World *world = new World(_vrWindow, 8, wantsPreview, false);
#endif

		RN::SceneManager::GetSharedInstance()->AddScene(world);
	}
}
