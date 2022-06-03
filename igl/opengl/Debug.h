#pragma once

#include <iostream>

#ifdef NDEBUG
#define debug(...) ((void)0)
#else
#define debug(...) debug_print(__FILE__, ":", __LINE__, ":", __FUNCTION__, ": ", __VA_ARGS__)
#endif

static void debug_print()
{
	std::cerr << std::endl;
}

template <typename T, typename ... Ts>
static void debug_print(T arg, Ts&&... args)
{
	std::cerr << arg;
	debug_print(args...);
}
