#pragma once

#include "pch.h"

#include "Events.h"
#include "IDrawable.h"
#include "IUpdate.h"

namespace CW {

	class Renderer;
	using namespace std::chrono_literals;

	/*enum class AppStatus
	{
		None = 0,
		TryingToCopy,
		SwappingBuffers
	};*/

	enum class AppStatus
	{
		None = 0,
		Updating,
		ReadyToCopy,
		CopyRequest
	};

	class Application
		: public IDrawable,
		  public IUpdate,
		  public virtual OnEvent
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		virtual void UpdateInterface() = 0;
		virtual void PauseUpdate(sf::Time deltaTime) = 0;

		bool IsRunning() const;
		void Close();

		bool IsPaused() const;
		void SwitchPause();

		size_t GetUPSLimit() const { return m_UPSLimit; }

		virtual void CollectState(Renderer& renderer) = 0;
		AppStatus GetStatus() const { return m_Status; }
		void RequestCopy()
		{
			m_Status = AppStatus::CopyRequest;
		}

	protected:
		bool m_Running = true;
		bool m_Pause = false;

		size_t m_UPSLimit = 60;
		std::atomic<AppStatus> m_Status = AppStatus::None;
	};

} // CW