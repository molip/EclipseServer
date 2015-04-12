#pragma once

#include <cassert>

extern bool _DebugBreak();

#ifdef _DEBUG
	#define ASSERT(f) (bool) ((f) || _DebugBreak())
#else
	#define ASSERT(f) (f)
#endif

#include "FormatString.h"

class Exception : public std::runtime_error
{
public:
	enum class Type { Generic, Input, Model, Serial };

	Exception(const std::string& msg, Type type) : std::runtime_error(msg), m_type(type) {}
	std::string GetTypeString() const;

private:
	Type m_type;
};

extern void Verify(const std::string& msg, Exception::Type type, const void* condition);
extern void Verify(const std::string& msg, Exception::Type type, bool condition);

#define _VERIFY(condition, type, msg) \
	::Verify(::FormatString("[%0:%1] %2", (strrchr(__FILE__,'\\') + 1), __LINE__, msg), type, condition)

#define VERIFY(condition) _VERIFY(condition, Exception::Type::Generic, "")
#define VERIFY_INPUT(condition) _VERIFY(condition, Exception::Type::Input, "")
#define VERIFY_MODEL(condition) _VERIFY(condition, Exception::Type::Model, "")
#define VERIFY_SERIAL(condition) _VERIFY(condition, Exception::Type::Serial, "")

#define VERIFY_MSG(msg, condition) _VERIFY(condition, Exception::Type::Generic, msg)
#define VERIFY_INPUT_MSG(msg, condition) _VERIFY(condition, Exception::Type::Input, msg)
#define VERIFY_MODEL_MSG(msg, condition) _VERIFY(condition, Exception::Type::Model, msg)
#define VERIFY_SERIAL_MSG(msg, condition) _VERIFY(condition, Exception::Type::Serial, msg)

template <typename T2, typename T1>
T2& VerifyCastInput(const T1& obj)
{
	T2* pObj = dynamic_cast<T2*>(&obj);
	VERIFY_INPUT_MSG(::FormatString("VerifyCastInput: Expected %0, got %1", typeid(T2).name(), typeid(T1).name()), !!pObj);
	return *pObj;
}
