#pragma once

#include "Aspect/Core/PlatformDetection.h"

#ifdef AS_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#ifdef AS_PLATFORM_WINDOWS
#include <Windows.h>
#endif


#include <iostream>
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Aspect/Core/Application.h>
#include <Aspect/Core/Assert.h>
#include <Aspect/Core/Base/Base.h>
#include <Aspect/Events/Event.h>
#include <Aspect/Core/Log.h>
//#include <Aspect/Core/Math/Mat4.h>
#include <Aspect/Core/Memory.h>
//#include <Aspect/Core/Delegate.h>

#include <Aspect/Debug/Instrumentor.h>


