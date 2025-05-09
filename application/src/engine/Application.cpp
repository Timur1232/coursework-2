#include "pch.h"
#include "Application.h"

#include "debug_utils/Log.h"

namespace CW {

	bool Application::IsRunning() const
	{
		return m_Running;
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::IsPaused() const
	{
		return m_Pause;
	}

	void Application::SwitchPause()
	{
		m_Pause = !m_Pause;
	}

} // CW