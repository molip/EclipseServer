#include "stdafx.h"
#include "Invitations.h"

#include "App.h"

#include "libKernel/Filesystem.h"
#include "libKernel/Util.h"

bool Invitations::Find(const std::string& name) 
{
	auto files = Kernel::FileSystem::FindFilesInDir(GetPath(), Kernel::StringToWString(name));
	return !files.empty();
}

bool Invitations::Remove(const std::string& name)
{
	return Kernel::FileSystem::DeleteFile(GetPath() + Kernel::StringToWString(name));
}

std::wstring Invitations::GetPath()
{
	return L"data/invitations/";
}
