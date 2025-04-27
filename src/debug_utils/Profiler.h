#pragma once

#include "pch.h"

#include "debug_utils/Log.h"

namespace CW {

	struct ProfileResult
	{
		std::string Name;
		std::chrono::duration<double, std::micro> Start;
		std::chrono::microseconds Duration;
	};

	class Profiler
	{
	public:
		Profiler(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;

		void StartSession(const std::string& filepath = "results.json")
		{
			m_OutFile.open(filepath);
			if (!m_OutFile)
			{
				CW_ERROR("Unable to open {} to start profiling session", filepath);
				return;
			}

			// Заголовок json файла
			m_OutFile << "{\"otherData\": {},\"traceEvents\":[{}";
		}

		void EndSession()
		{
			if (m_OutFile)
			{
				// Конец json файла
				m_OutFile << "],\"displayTimeUnit\": \"us\"}";
				m_OutFile.close();
			}
		}
		
		void WriteProfile(const ProfileResult& result)
		{
			if (!m_OutFile)
			{
				return;
			}

			std::stringstream json;

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.Duration.count()) << ',';
			json << "\"name\":\"" << result.Name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"ts\":" << result.Start.count();
			json << "}";

			m_OutFile << json.str();
		}
		
		static Profiler& Get()
		{
			static Profiler instance;
			return instance;
		}

	private:
		Profiler() = default;
		~Profiler()
		{
			EndSession();
		}

	private:
		std::ofstream m_OutFile;
	};

	class ScopeTimer
	{
	public:
		ScopeTimer(const char* name)
			: m_Name(name), m_Stopped(false)
		{
			m_StartPoint = std::chrono::steady_clock::now();
		}

		~ScopeTimer()
		{
			Stop();
		}

		void Stop()
		{
			if (!m_Stopped)
			{
				auto startPoint = std::chrono::duration<double, std::micro>{m_StartPoint.time_since_epoch()};
				auto endPoint = std::chrono::steady_clock::now();
				auto duration = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch();;

				Profiler::Get().WriteProfile({m_Name, startPoint, duration});

				m_Stopped = true;
			}
		}

	private:
		std::string m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartPoint;
		bool m_Stopped;
	};

} // CW

#define CW_PROFILING 1
#if CW_PROFILING
	#define CW_START_PROFILE_SESSION(name) ::CW::Profiler::Get().StartSession(name);
	#define CW_END_PROFILE_SESSION() ::CW::Profiler::Get().EndSession();
	#define CW_PROFILE_SCOPE(name) ::CW::ScopeTimer timer(name)
	#define CW_PROFILE_FUNCTION() CW_PROFILE_SCOPE(__FUNCTION__)
#else
	#define CW_START_PROFILE_SESSION(name)
	#define CW_END_PROFILE_SESSION()
	#define CW_PROFILE_SCOPE(name)
	#define CW_PROFILE_FUNCTION()
#endif