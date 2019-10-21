#pragma once

#include <string>

class Invitations
{
public:
	static std::wstring GetPath();

	static bool Find(const std::string& name);
	static bool Remove(const std::string& name);
};

