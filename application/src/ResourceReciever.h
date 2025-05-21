#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"
#include "DroneSettings.h"

namespace CW {

	class ResourceReciever
		: public Object,
		  public IDrawable,
		  public IUpdate
	{
	public:
		ResourceReciever(sf::Vector2f position, int droneCost, float spawnCooldown);
		ResourceReciever(const DroneSettings& settings);

		void DebugInterface() const;

		void Draw() override;
		void Update(float deltaTime) override;

		[[nodiscard]] int GetResources() const;
		[[nodiscard]] float GetBroadcastRadius() const;
		[[nodiscard]] float GetRecieveRadius() const;

		void AddResources(int amount);
		[[nodiscard]] int TakeResources(int amount);

	private:
		int m_ResourceCount = 0;
		float m_RecieveRadius;
		float m_BroadcastRadius;

		int m_DroneCost;
		float m_DroneSpawnTimer = 0.0f;
		float m_DroneSpawnCooldown;
	};

} // CW