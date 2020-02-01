#include "PSGrabbable.h"

namespace PS {

	RNDefineMeta(Grabbable, RN::Entity)

	void Grabbable::SetIsGrabbed(bool isGrabbed)
	{
		if (_isGrabbed != isGrabbed)
		{
			if (isGrabbed)
			{
				_currentGrabbedSpeed = RN::Vector3();
			}
			_wantsThrow = !isGrabbed;
		}
		_isGrabbed = isGrabbed;
	}

	void Grabbable::Update(float delta)
	{
		if (IsGrabbed())
		{
			RN::Vector3 currentSpeed = (GetWorldPosition() - _previousPosition) / delta;
			_currentGrabbedSpeed = _currentGrabbedSpeed.GetLerp(currentSpeed, 0.5f);
		}

		_previousPosition = GetWorldPosition();
	}

	void Grabbable::SetIsTriggered(bool isTriggered)
	{
		_isTriggered = isTriggered;
	}
}