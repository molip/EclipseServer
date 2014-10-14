#include "OS.h"
#include "App.h"

#include <windows.h>

namespace OS
{

std::vector<std::string> FindFilesInDir(const std::string& dir, const std::string& pattern)
{
	std::vector<std::string> files;

	std::string dir2 = dir;
	if (!dir2.empty() && dir2.back() != '/')
		dir2.push_back('/');
	dir2 += pattern;

	WIN32_FIND_DATAA ffd;
	HANDLE hFind = FindFirstFileA(dir2.c_str(), &ffd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			files.push_back(ffd.cFileName);
		}
		while (FindNextFileA(hFind, &ffd) != 0);
 
		FindClose(hFind);
	}
	return files;
}
#undef DeleteFile
bool DeleteFile(const std::string& path)
{
	return ::DeleteFileA(path.c_str()) != 0;
}

} // namespace