#pragma once

#include "Aspect/Core/PlatformDetection.h"
#include <memory>

/*
#ifdef  AS_PLATFORM_WINDOWS
#ifdef AS_DYNAMIC_LINK
	#ifdef AS_BUILD_DLL
		#define ASPECT_API __declspec(dllexport)
	#else 
		#define ASPECT_API __declspec(dllimport)
	#endif // AS_BUILD_DLL
#else
	#define ASPECT_API
#endif
#else
	#error Aspect only support Windows!
#endif //  AS_PLATFORM_WINDOWS


#ifdef AS_DEBUG
	#define AS_ENABLE_ASSERTS
#endif

#ifdef AS_ENABLE_ASSERTS
	#define AS_ASSERT(x, ...) { if(!(x)) { AS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define AS_CORE_ASSERT(x, ...) { if(!(x)) { AS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define AS_ASSERT(x, ...)
	#define AS_CORE_ASSERT(x, ...)
#endif*/

#ifdef AS_DEBUG
#if defined(AS_PLATFORM_WINDOWS)
#define AS_DEBUGBREAK() __debugbreak()
#elif defined(AS_PLATFORM_LINUX)
#include <signal.h>
#define AS_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define AS_ENABLE_ASSERTS
#else
#define AS_DEBUGBREAK()
#endif

#define AS_EXPAND_MACRO(x) x
#define AS_STRINGIFY_MACRO(x) #x


//#define BIT(x) (1<<x)

//#define AS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define BIT(x) (1 << x)

//todo:
#define AS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef AS_PLATFORM_WINDOWS
#define AS_FORCE_INLINE __forceinline
#else
	// TODO: other platforms
#define AS_FORCE_INLINE inline
#endif


//=====AS MACRO======
#define AS_XSTRINGIFY_MACRO(x) AS_STRINGIFY_MACRO(x)
#define AS_STRINGIFY_MACRO(x) #x

namespace Aspect
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	using byte = uint8_t;

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	//using byte = uint8_t;

	/** A simple wrapper for std::atomic_flag to avoid confusing
		function names usage. The object owning it can still be
		default copyable, but the copied flag is going to be reset.
	*/
	/*struct AtomicFlag
	{
		AS_FORCE_INLINE void SetDirty() { flag.clear(); }
		AS_FORCE_INLINE bool CheckAndResetIfDirty() { return !flag.test_and_set(); }

		explicit AtomicFlag() noexcept { flag.test_and_set(); }
		AtomicFlag(const AtomicFlag&) noexcept {}
		AtomicFlag& operator=(const AtomicFlag&) noexcept { return *this; }
		AtomicFlag(AtomicFlag&&) noexcept {};
		AtomicFlag& operator=(AtomicFlag&&) noexcept { return *this; }

	private:
		std::atomic_flag flag;
	};

	struct Flag
	{
		AS_FORCE_INLINE void SetDirty() noexcept { flag = true; }
		AS_FORCE_INLINE bool CheckAndResetIfDirty() noexcept
		{
			if (flag)
				return !(flag = !flag);
			else
				return false;
		}

		AS_FORCE_INLINE bool IsDirty() const noexcept { return flag; }

	private:
		bool flag = false;
	};*/
}


#include "Aspect/Core/Log.h"
#include "Aspect/Core/Assert.h"
