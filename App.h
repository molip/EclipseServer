#pragma once

#include <map>
#include <vector>
#include <set>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>

//#define LOCK(m) ; //std::lock_guard<std::mutex> lock(m);
#define LOCK(m) std::lock_guard<std::mutex> lock(m);

extern bool _DebugBreak();

#ifdef _DEBUG
	#define ASSERT(f) (bool) ((f) || _DebugBreak())
#else
	#define ASSERT(f) (true)
#endif
