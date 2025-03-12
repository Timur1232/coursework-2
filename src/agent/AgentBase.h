#pragma once

#include <glm/glm.hpp>

class AgentBase
{
public:
	AgentBase() = default;
	AgentBase(glm::vec2 position);

	virtual void draw() = 0;

private:
	glm::vec2 m_Position;
	// uint32_t m_InventorySlot;
};