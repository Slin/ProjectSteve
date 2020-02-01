#pragma once

#include <Rayne.h>

namespace PS {

	class Spawner;
	class Grabbable : public RN::Entity
	{
	public:
	//	template<typename... Args>
	//	Grabbable(Args&&... args) : Entity(std::forward<Args>(args)), _isGrabbed(false) {}
		using Entity::Entity;
		void Update(float delta) override;

		void SetIsGrabbed(bool isGrabbed);
		bool IsGrabbed() const { return _isGrabbed; }

		void SetIsTriggered(bool isTriggered);
		virtual void Reset();
		
		void SetSpawner(Spawner *spawner) { _spawner = spawner; }

	protected:
		bool _isGrabbed = false;
		bool _wantsThrow = false;
		bool _isTriggered = false;

		RN::Vector3 _previousPosition;
		RN::Vector3 _currentGrabbedSpeed;
		RN::Vector3 _currentGrabbedRotationSpeed;
		
		Spawner *_spawner;

		RNDeclareMeta(Grabbable)
	};
}
