#include "App.h"
#include "EnumTraits.h"

#include <windows.h>

bool _DebugBreak() { DebugBreak(); return false; }

void Verify(const std::string& msg, Exception::Type type, const void* condition)
{
	Verify(msg, type, condition != nullptr);
}

void Verify(const std::string& msg, Exception::Type type, bool condition)
{
	if (!ASSERT(condition))
		throw Exception(msg, type);
}

std::string _FormatFileAndLine(const char* file, int line)
{
	const char* trimmed = strrchr(__FILE__, '\\');
	trimmed = trimmed ? trimmed + 1 : file;
	return ::FormatString("[%0:%1] ", trimmed, line);
}

DEFINE_ENUM_NAMES(Exception::Type) { "Generic", "Input", "Model", "Serial", "" };

std::string Exception::GetTypeString() const
{
	return EnumToString(m_type);
}