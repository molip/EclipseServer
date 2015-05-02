#include "Util.h"

#include <algorithm>
#include <cctype>

namespace Util
{

std::vector<std::string> SplitString(const std::string& str, char sep)
{
	std::vector<std::string> v;
	std::string s;
	for (char c : str)
	{
		if (c == sep)
		{
			v.push_back(s);
			s.clear();
		}
		else
			s.push_back(c);
	}
	if (!s.empty())
		v.push_back(s);

	return v;
}

std::string ReplaceAll(const std::string& str, const std::string& oldSubStr, const std::string& newSubStr)
{
	std::string result;
	size_t pos = 0;
	
	while (pos != str.length())
	{
		size_t found = str.find(oldSubStr, pos);
		if (found == std::string::npos)
		{
			result += str.substr(pos);
			break;
		}
		result += str.substr(pos, found - pos);
		result += newSubStr;
		pos = found + oldSubStr.length();
	}
	return result;
}

void MakeLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), std::tolower);
}

std::string ToLower(const std::string& str)
{
	std::string lower = str;
	MakeLower(lower);
	return lower;
}

}