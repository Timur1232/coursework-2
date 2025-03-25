#include "Events.h"

#include <imgui-SFML.h>

CW::EventHandler::EventHandler(size_t reserve)
{
	m_EventSubscribers.reserve(reserve);
}

void CW::EventHandler::subscribe(IOnEvent* newSubscriber)
{
	m_EventSubscribers.push_back(newSubscriber);
}

void CW::EventHandler::handleEvents(sf::RenderWindow& window)
{
	while (const std::optional event = window.pollEvent())
	{
		ImGui::SFML::ProcessEvent(window, *event);
		for (auto it : m_EventSubscribers)
		{
			it->onEvent(*event);
		}
	}
}
