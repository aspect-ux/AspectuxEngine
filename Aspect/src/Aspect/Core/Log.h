#pragma once

#include "Aspect/Core/Base/Base.h"

#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Aspect {

	class Log
	{
	public:
		enum class Type : uint8_t
		{
			Core = 0, Client = 1
		};
		enum class Level : uint8_t
		{
			Trace = 0, Info, Warn, Error, Fatal
		};
		struct TagDetails
		{
			bool Enabled = true;
			Level LevelFilter = Level::Trace;
		};
	public:
		static void Init();
		static void Shutdown();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetEditorConsoleLogger() { return s_EditorConsoleLogger; }

		static bool HasTag(const std::string& tag) { return s_EnabledTags.find(tag) != s_EnabledTags.end(); }
		static std::map<std::string, TagDetails>& EnabledTags() { return s_EnabledTags; }

		template<typename... Args>
		static void PrintMessage(Log::Type type, Log::Level level, std::string_view tag, Args&&... args);

		template<typename... Args>
		static void PrintAssertMessage(Log::Type type, std::string_view prefix, Args&&... args);

	public:
		// Enum utils
		static const char* LevelToString(Level level)
		{
			switch (level)
			{
			case Level::Trace: return "Trace";
			case Level::Info:  return "Info";
			case Level::Warn:  return "Warn";
			case Level::Error: return "Error";
			case Level::Fatal: return "Fatal";
			}
			return "";
		}
		static Level LevelFromString(std::string_view string)
		{
			if (string == "Trace") return Level::Trace;
			if (string == "Info")  return Level::Info;
			if (string == "Warn")  return Level::Warn;
			if (string == "Error") return Level::Error;
			if (string == "Fatal") return Level::Fatal;

			return Level::Trace;
		}
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_EditorConsoleLogger;

		inline static std::map<std::string, TagDetails> s_EnabledTags;
	};

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging
#define AS_CORE_TRACE_TAG(tag, ...) ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Core, ::Aspect::Log::Level::Trace, tag, __VA_ARGS__)
#define AS_CORE_INFO_TAG(tag, ...)  ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Core, ::Aspect::Log::Level::Info, tag, __VA_ARGS__)
#define AS_CORE_WARN_TAG(tag, ...)  ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Core, ::Aspect::Log::Level::Warn, tag, __VA_ARGS__)
#define AS_CORE_ERROR_TAG(tag, ...) ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Core, ::Aspect::Log::Level::Error, tag, __VA_ARGS__)
#define AS_CORE_FATAL_TAG(tag, ...) ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Core, ::Aspect::Log::Level::Fatal, tag, __VA_ARGS__)

// Client logging
#define AS_TRACE_TAG(tag, ...) ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Client, ::Aspect::Log::Level::Trace, tag, __VA_ARGS__)
#define AS_INFO_TAG(tag, ...)  ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Client, ::Aspect::Log::Level::Info, tag, __VA_ARGS__)
#define AS_WARN_TAG(tag, ...)  ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Client, ::Aspect::Log::Level::Warn, tag, __VA_ARGS__)
#define AS_ERROR_TAG(tag, ...) ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Client, ::Aspect::Log::Level::Error, tag, __VA_ARGS__)
#define AS_FATAL_TAG(tag, ...) ::Aspect::Log::PrintMessage(::Aspect::Log::Type::Client, ::Aspect::Log::Level::Fatal, tag, __VA_ARGS__)

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

// Core log macros
#define AS_CORE_TRACE(...)    ::Aspect::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AS_CORE_INFO(...)     ::Aspect::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AS_CORE_WARN(...)     ::Aspect::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AS_CORE_ERROR(...)    ::Aspect::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AS_CORE_CRITICAL(...) ::Aspect::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define AS_TRACE(...)         ::Aspect::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AS_INFO(...)          ::Aspect::Log::GetClientLogger()->info(__VA_ARGS__)
#define AS_WARN(...)          ::Aspect::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AS_ERROR(...)         ::Aspect::Log::GetClientLogger()->error(__VA_ARGS__)
#define AS_CRITICAL(...)      ::Aspect::Log::GetClientLogger()->critical(__VA_ARGS__)


// Editor Console Logging Macros
#define AS_CONSOLE_LOG_TRACE(...)   Aspect::Log::GetEditorConsoleLogger()->trace(__VA_ARGS__)
#define AS_CONSOLE_LOG_INFO(...)    Aspect::Log::GetEditorConsoleLogger()->info(__VA_ARGS__)
#define AS_CONSOLE_LOG_WARN(...)    Aspect::Log::GetEditorConsoleLogger()->warn(__VA_ARGS__)
#define AS_CONSOLE_LOG_ERROR(...)   Aspect::Log::GetEditorConsoleLogger()->error(__VA_ARGS__)
#define AS_CONSOLE_LOG_FATAL(...)   Aspect::Log::GetEditorConsoleLogger()->critical(__VA_ARGS__)


namespace Aspect {
	template<typename... Args>
	void Log::PrintMessage(Log::Type type, Log::Level level, std::string_view tag, Args&&... args)
	{
		auto detail = s_EnabledTags[std::string(tag)];
		if (detail.Enabled && detail.LevelFilter <= level)
		{
			auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
			std::string logString = tag.empty() ? "{0}{1}" : "[{0}] {1}";
			switch (level)
			{
			case Level::Trace:
				logger->trace(logString, tag, fmt::format(std::forward<Args>(args)...));
				break;
			case Level::Info:
				logger->info(logString, tag, fmt::format(std::forward<Args>(args)...));
				break;
			case Level::Warn:
				logger->warn(logString, tag, fmt::format(std::forward<Args>(args)...));
				break;
			case Level::Error:
				logger->error(logString, tag, fmt::format(std::forward<Args>(args)...));
				break;
			case Level::Fatal:
				logger->critical(logString, tag, fmt::format(std::forward<Args>(args)...));
				break;
			}
		}
	}


	template<typename... Args>
	void Log::PrintAssertMessage(Log::Type type, std::string_view prefix, Args&&... args)
	{
		auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
		logger->error("{0}: {1}", prefix, fmt::format(std::forward<Args>(args)...));

#if AS_ASSERT_MESSAGE_BOX
		std::string message = fmt::format(std::forward<Args>(args)...);
		MessageBoxA(nullptr, message.c_str(), "Aspect Assert", MB_OK | MB_ICONERROR);
#endif
	}

	template<>
	inline void Log::PrintAssertMessage(Log::Type type, std::string_view prefix)
	{
		auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
		logger->error("{0}", prefix);
#if AS_ASSERT_MESSAGE_BOX
		MessageBoxA(nullptr, "No message :(", "Aspect Assert", MB_OK | MB_ICONERROR);
#endif
	}
}
