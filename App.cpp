#include "App.h"
#include <windows.h>

bool _DebugBreak() { DebugBreak(); return false; }

std::string FormatInt(int n)
{
	std::ostringstream ss;
	ss << n;
	return ss.str();
}

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

void AssertThrowModel(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw ModelException(msg);
}
