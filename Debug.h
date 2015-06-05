#pragma once

#include "libKernel/Debug.h"

#define ASSERT(f) KERNEL_ASSERT(f)

class AppException : public Exception
{
public:
	enum class Type { Generic, Input, Model, Serial };

	AppException(const char* file, int line, Type type, const char* msg) : Exception(file, line, msg), m_type(type) {}
	AppException(const char* file, int line, Type type, const std::string& msg) : Exception(file, line, msg), m_type(type) {}

	virtual void Raise() const { throw *this; }

	std::string GetTypeString() const;

protected:
	virtual std::string GetWhat() const override;

private:
	Type m_type;
};

#define _VERIFY(condition, type, msg) \
	Verify(condition, AppException(__FILE__, __LINE__, type, msg))

#define VERIFY(condition) _VERIFY(condition, AppException::Type::Generic, "")
#define VERIFY_INPUT(condition) _VERIFY(condition, AppException::Type::Input, "")
#define VERIFY_MODEL(condition) _VERIFY(condition, AppException::Type::Model, "")
#define VERIFY_SERIAL(condition) _VERIFY(condition, AppException::Type::Serial, "")

#define VERIFY_MSG(msg, condition) _VERIFY(condition, AppException::Type::Generic, msg)
#define VERIFY_INPUT_MSG(msg, condition) _VERIFY(condition, AppException::Type::Input, msg)
#define VERIFY_MODEL_MSG(msg, condition) _VERIFY(condition, AppException::Type::Model, msg)
#define VERIFY_SERIAL_MSG(msg, condition) _VERIFY(condition, AppException::Type::Serial, msg)

template <typename T2, typename T1>
T2& VerifyCastInput(const T1& obj)
{
	T2* pObj = dynamic_cast<T2*>(&obj);
	VERIFY_INPUT_MSG(::FormatString("VerifyCastInput: Expected %0, got %1", typeid(T2).name(), typeid(T1).name()), !!pObj);
	return *pObj;
}
