#pragma once

#include <string>
#include <vector>

namespace OS
{

std::vector<std::string> FindFilesInDir(const std::string& dir, const std::string& pattern = "*");
	
}