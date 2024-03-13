#pragma once

#define AS_ENABLE_PROFILING !AS_DIST

#if AS_ENABLE_PROFILING 
#include <optick.h>
#endif

#if AS_ENABLE_PROFILING
#define AS_PROFILE_FRAME(...)           OPTICK_FRAME(__VA_ARGS__)
#define AS_PROFILE_FUNC(...)            OPTICK_EVENT(__VA_ARGS__)
#define AS_PROFILE_TAG(NAME, ...)       OPTICK_TAG(NAME, __VA_ARGS__)
#define AS_PROFILE_SCOPE_DYNAMIC(NAME)  OPTICK_EVENT_DYNAMIC(NAME)
#define AS_PROFILE_THREAD(...)          OPTICK_THREAD(__VA_ARGS__)
#else
#define AS_PROFILE_FRAME(...)
#define AS_PROFILE_FUNC(...)
#define AS_PROFILE_TAG(NAME, ...) 
#define AS_PROFILE_SCOPE_DYNAMIC(NAME)
#define AS_PROFILE_THREAD(...)
#endif
