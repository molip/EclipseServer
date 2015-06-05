#include "stdafx.h"
#include "Invitations.h"

#include "App.h"

#include "libKernel/Filesystem.h"

bool Invitations::Find(const std::string& name) 
{
	auto files = Kernel::FindFilesInDir(GetPath(), name);
	return !files.empty();
}

bool Invitations::Remove(const std::string& name)
{
	return Kernel::DeleteFile(GetPath() + name);
}

std::string Invitations::GetPath()
{
	return "data/invitations/";
}
