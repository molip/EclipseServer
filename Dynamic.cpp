#include "Dynamic.h"

Dynamic::~Dynamic() {}

Dynamic::Map& Dynamic::GetMap()
{
	static Map map;
	return map;
}

void Dynamic::RegisterObject(const std::string& id, std::function<Dynamic*()> fn)
{
	GetMap().insert(std::make_pair(id, fn));
}
