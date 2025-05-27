#pragma once
#include "pch.h"

namespace CW {

	namespace Log {

		constexpr auto RESET     = "\033[0m";
		constexpr auto RED       = "\033[31m";
		constexpr auto GREEN     = "\033[32m";
		constexpr auto YELLOW    = "\033[33m";
		constexpr auto BLUE      = "\033[34m";
		constexpr auto MAGENTA   = "\033[35m";
		constexpr auto CYAN      = "\033[36m";
		constexpr auto WHITE     = "\033[37m";

		constexpr auto BOLD      = "\033[1m";
		constexpr auto UNDERLINE = "\033[4m";
		constexpr auto REVERSE   = "\033[7m";

		template<typename... Args>
		void trace(const std::format_string<Args...> format, Args&&... args)
		{
			std::osyncstream(std::cout) << std::format(format, std::forward<Args>(args)...) << std::endl;
		}

		template<typename... Args>
		void error(const std::format_string<Args...> format, Args&&... args)
		{
			std::osyncstream(std::cout) << RED << std::format(format, std::forward<Args>(args)...) << RESET << std::endl;
		}

		template<typename... Args>
		void info(const std::format_string<Args...> format, Args&&... args)
		{
			std::osyncstream(std::cout) << GREEN << std::format(format, std::forward<Args>(args)...) << RESET << std::endl;
		}

		template<typename... Args>
		void warn(const std::format_string<Args...> format, Args&&... args)
		{
			std::osyncstream(std::cout) << YELLOW << std::format(format, std::forward<Args>(args)...) << RESET << std::endl;
		}

		template<typename... Args>
		void critical(const std::format_string<Args...> format, Args&&... args)
		{
			std::osyncstream(std::cout) << RED << REVERSE << std::format(format, std::forward<Args>(args)...) << RESET << std::endl;
		}

	} // Log

} // CW

#define CW_COLLECT_LOGS 1
#if CW_COLLECT_LOGS
	#define CW_MSG(format, ...) ::CW::Log::trace("[MSG] " format, __VA_ARGS__)
	#define CW_TRACE(format, ...) ::CW::Log::trace("[TRACE] [{} : {}] ({}) :: " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define CW_ERROR(format, ...) ::CW::Log::error("[ERROR] [{} : {}] ({}) :: " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define CW_INFO(format, ...) ::CW::Log::info("[INFO] [{} : {}] ({}) :: " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define CW_WARN(format, ...) ::CW::Log::warn("[WARN] [{} : {}] ({}) :: " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define CW_CRITICAL(format, ...) ::CW::Log::critical("[CRITICAL] [{} : {}] ({}) :: " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define CW_ASSERT(format, ...) {::CW::Log::critical("[ASSERT] [{} : {}] ({}) :: " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); exit(-1);}
#else
	#define CW_TRACE(format, ...)
	#define CW_ERROR(format, ...)
	#define CW_INFO(format, ...)
	#define CW_WARN(format, ...)
	#define CW_CRITICAL(format, ...)
#endif