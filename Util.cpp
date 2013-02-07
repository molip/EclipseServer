#include "Util.h"

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

}