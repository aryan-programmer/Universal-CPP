// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS

#include <list>
#include <deque>
#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <functional>
#include <string_view>
#include <unordered_map>

#include <boost\config.hpp>
#include <boost\thread.hpp>
#include <boost\variant.hpp>
#include <boost\scope_exit.hpp>
#include <boost\stacktrace.hpp>
#include <boost\variant\get.hpp>
#include <boost\unordered_map.hpp>
#include <boost\container\deque.hpp>
#include <boost\container\vector.hpp>
#include <boost\algorithm\string.hpp>
#include <boost\type_traits\add_const.hpp>
#include <boost\type_traits\is_detected.hpp>
#include <boost\type_traits\add_reference.hpp>
#include <boost\type_traits\remove_cv_ref.hpp>
#include <boost\type_traits\remove_cv_ref.hpp>
#include <boost\type_traits\remove_reference.hpp>

#define forceinline BOOST_FORCEINLINE

// reference additional headers your program requires here
