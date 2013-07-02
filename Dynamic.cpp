#include "Dynamic.h"

Dynamic::~Dynamic() {}

Dynamic::ClassMap& Dynamic::GetClassMap()
{
	static ClassMap map;
	return map;
}

void Dynamic::RegisterObject(const std::string& id, std::function<Dynamic*()> fn)
{
	GetClassMap().insert(std::make_pair(id, fn));
}
