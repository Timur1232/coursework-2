#include "IUpdate.h"

#include "debug_utils/Log.h"

namespace CW_E {

	UpdateHandler::UpdateHandler(size_t reserve)
	{
		m_UpdateTargets.reserve(reserve);
	}

	size_t UpdateHandler::subscribe(IUpdate* target)
	{
		if (m_HasUnsubs)
		{
			for (size_t i = 0; i < m_UpdateTargets.size(); i++)
			{
				if (!m_UpdateTargets[i])
				{
					m_UpdateTargets[i] = target;
					return i;
				}
			}
			m_HasUnsubs = false;
		}
		m_UpdateTargets.emplace_back(target);
		return m_UpdateTargets.size() - 1;
	}

	void UpdateHandler::unsubscribe(size_t index)
	{
		m_UpdateTargets[index] = nullptr;
		m_HasUnsubs = true;
	}

	void UpdateHandler::handleUpdates(sf::Time deltaTime)
	{
		for (auto target : m_UpdateTargets)
		{
			if (target)
				target->update(deltaTime);
		}
	}

	UpdateHandlerWrapper::UpdateHandlerWrapper(UpdateHandler* handler)
		: m_UpdateHandler(handler)
	{
	}

	size_t UpdateHandlerWrapper::subscribe(IUpdate* target)
	{
		return m_UpdateHandler->subscribe(target);
	}

	void UpdateHandlerWrapper::unsubscribe(size_t index)
	{
		m_UpdateHandler->unsubscribe(index);
	}

}