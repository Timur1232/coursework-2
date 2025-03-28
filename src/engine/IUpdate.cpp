#include "IUpdate.h"

#include "debug_utils/Log.h"

namespace CW {

	UpdateHandler::UpdateHandler(size_t reserve)
	{
		m_UpdateTargets.reserve(reserve);
	}

	void UpdateHandler::subscribe(IUpdate* target)
	{
		CW_MSG("Target subscribed with address: {}", (void*)target);
		m_UpdateTargets.emplace_back(target);
	}

	void UpdateHandler::handleUpdates()
	{
		for (auto target : m_UpdateTargets)
		{
			target->update();
		}
	}

	UpdateHandlerWrapper::UpdateHandlerWrapper(UpdateHandler* handler)
		: m_UpdateHandler(handler)
	{
	}

	void UpdateHandlerWrapper::subscribe(IUpdate* target)
	{
		m_UpdateHandler->subscribe(target);
	}

}