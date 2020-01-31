//
//  PSApplication.h
//  Project Z
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_APPLICATION_H_
#define __ProjectSteve_APPLICATION_H_

#include <Rayne.h>

using namespace RN::numeric;

namespace RN
{
	class VRWindow;
}

namespace PS
{
	class Application : public RN::Application
	{
	public:
		Application();
		~Application();

		void WillFinishLaunching(RN::Kernel *kernel) override;
		void DidFinishLaunching(RN::Kernel *kernel) override;

		RN::VRWindow *GetVRWindow() const { return _vrWindow; }

		RN::RendererDescriptor *GetPreferredRenderer() const final;
		RN::RenderingDevice *GetPreferredRenderingDevice(RN::RendererDescriptor *descriptor, const RN::Array *devices) const final;

	private:
		void SetupVR();
		RN::VRWindow *_vrWindow;
	};
}


#endif /* __ProjectSteve_APPLICATION_H_ */
