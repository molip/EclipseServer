#include "stdafx.h"
#include "Debug.h"

DEFINE_ENUM_NAMES(AppException::Type) { "Generic", "Input", "Model", "Serial", "" };

std::string AppException::GetTypeString() const
{
	return EnumToString(m_type);
}
std::string AppException::GetWhat() const
{
	return GetTypeString() + ' ' + __super::GetWhat();
}

void AppVerify(const void* condition, const Kernel::Exception& e)
{
	AppVerify(condition != nullptr, e);
}

void AppVerify(bool condition, const Kernel::Exception& e)
{
	KERNEL_ASSERT(condition);
	if (!condition)
		e.Raise();
}
