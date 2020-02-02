#include "PSGrabbable.h"
#include "PSSpawner.h"

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
		if(_isFirstUpdate)
		{
			_isFirstUpdate = false;
			_startPosition = GetWorldPosition();
		}
		
		if (IsGrabbed())
		{
			RN::Vector3 currentSpeed = (GetWorldPosition() - _previousPosition) / delta;
			_currentGrabbedSpeed = _currentGrabbedSpeed.GetLerp(currentSpeed, 0.5f);
		}

		_previousPosition = GetWorldPosition();
		
		if(GetWorldPosition().y < -2.0f)
		{
			if(_spawner) _spawner->ReturnToPool(this);
			else SetWorldPosition(_startPosition);
		}
	}

	void Grabbable::SetIsTriggered(bool isTriggered)
	{
		_isTriggered = isTriggered;
	}

	void Grabbable::Reset()
	{
		_isFirstUpdate = true;
	}
}
