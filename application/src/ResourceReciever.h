#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"
#include "DroneSettings.h"

namespace CW {

	struct RecieverData
	{
		int ResourceCount = 0;
		float RecieveRadius = 0.0f;
		float BroadcastRadius = 0.0f;
		int DroneCost = 0;
		float DroneSpawnTimer = 0.0f;
		float DroneSpawnCooldown = 0.0f;
	};

	class ResourceReciever
		: public Object,
		  public IDrawable,
		  public IUpdate
	{
	public:
		ResourceReciever() = default;
		ResourceReciever(sf::Vector2f position, int droneCost, float spawnCooldown);
		ResourceReciever(const DroneSettings& settings);

		void DebugInterface() const;

		RecieverData GetData() const;
		void SetData(const RecieverData& data);

		void Draw() override;
		void Update(float deltaTime) override;

		[[nodiscard]] int GetResources() const;
		[[nodiscard]] float GetBroadcastRadius() const;
		[[nodiscard]] float GetRecieveRadius() const;

		void AddResources(int amount);
		[[nodiscard]] int TakeResources(int amount);

	private:
		int m_ResourceCount = 0;
		float m_RecieveRadius = 0.0f;
		float m_BroadcastRadius = 0.0f;

		int m_DroneCost = 0;
		float m_DroneSpawnTimer = 0.0f;
		float m_DroneSpawnCooldown = 0.0f;
	};

} // CW