#include "Invitations.h"
#include "App.h"
#include "OS.h"

bool Invitations::Find(const std::string& name) 
{
	auto files = OS::FindFilesInDir(GetPath(), name);
	return !files.empty();
}

bool Invitations::Remove(const std::string& name)
{
	return OS::DeleteFile(GetPath() + name);
}

std::string Invitations::GetPath()
{
	return "data/invitations/";
}
