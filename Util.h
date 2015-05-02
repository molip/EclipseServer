#pragma once

#include <string>
#include <vector>

namespace Util
{
	std::vector<std::string> SplitString(const std::string& str, char sep = ' ');
	std::string ReplaceAll(const std::string& str, const std::string& oldSubStr, const std::string& newSubStr);
	void MakeLower(std::string& str);
	std::string ToLower(const std::string& str);
}