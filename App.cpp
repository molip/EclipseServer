#include "App.h"
#include <windows.h>

bool _DebugBreak() { DebugBreak(); return false; }

void AssertThrow(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw GenericException(msg);
}

void AssertThrowXML(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw XMLException(msg);
}
