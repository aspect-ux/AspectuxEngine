#pragma once

#include "Aspect/Core/Base/Base.h"
#include "Aspect/Core/Log.h"
#include <filesystem>

#ifdef AS_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define AS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { AS##type##ERROR(msg, __VA_ARGS__); AS_DEBUGBREAK(); } }
	#define AS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) AS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define AS_INTERNAL_ASSERT_NO_MSG(type, check) AS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", AS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define AS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define AS_INTERNAL_ASSERT_GET_MACRO(...) AS_EXPAND_MACRO( AS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, AS_INTERNAL_ASSERT_WITH_MSG, AS_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define AS_ASSERT(...) AS_EXPAND_MACRO( AS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define AS_CORE_ASSERT(...) AS_EXPAND_MACRO( AS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define AS_ASSERT(...)
	#define AS_CORE_ASSERT(...)
#endif


#ifdef AS_ENABLE_VERIFY
	#define AS_CORE_VERIFY_MESSAGE_INTERNAL(...)  ::Aspect::Log::PrintAssertMessage(::Aspect::Log::Type::Core, "Verify Failed", __VA_ARGS__)
	#define AS_VERIFY_MESSAGE_INTERNAL(...)  ::Aspect::Log::PrintAssertMessage(::Aspect::Log::Type::Client, "Verify Failed", __VA_ARGS__)

	#define AS_CORE_VERIFY(condition, ...) { if(!(condition)) { AS_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); AS_DEBUG_BREAK; } }
	#define AS_VERIFY(condition, ...) { if(!(condition)) { AS_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); AS_DEBUG_BREAK; } }
#else
	#define AS_CORE_VERIFY(condition, ...)
	#define AS_VERIFY(condition, ...)
#endif

