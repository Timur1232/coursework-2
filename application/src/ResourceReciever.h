#pragma once

#include "pch.h"

#include "engine/IDrawable.h"
#include "Object.h"

namespace CW {

	class ResourceReciever
		: public Object,
		  public IDrawable
	{
	public:
		ResourceReciever(sf::Vector2f position);

		void DebugInterface() const;

		void Draw() override;

		[[nodiscard]] int GetResources() const;
		[[nodiscard]] float GetBroadcastRadius() const;
		[[nodiscard]] float GetRecieveRadius() const;

		void AddResources(int amount);
		[[nodiscard]] int TakeResources(int amount);

	private:
		int m_ResourceCount = 0;
		float m_RecieveRadius;
		float m_BroadcastRadius;
	};

} // CW