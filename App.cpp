#include "App.h"
#include "Resources.h"
#include "Hex.h"

#include <windows.h>

bool _DebugBreak() { DebugBreak(); return false; }

std::string FormatInt(int n)
{
	std::ostringstream ss;
	ss << n;
	return ss.str();
}

int Mod(int a, int b)
{
	return a % b + (a < 0 ? b : 0);
}

void Verify(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw GenericException(msg);
}

void VerifyInput(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw XMLException(msg);
}

void VerifyModel(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw ModelException(msg);
}

void VerifySerial(const std::string& msg, bool b)
{
	if (!ASSERT(b))
		throw SerialException(msg);
}

Resource SquareTypeToResource(SquareType t)
{
	switch (t)
	{
	case SquareType::Money: return Resource::Money;
	case SquareType::Science: return Resource::Science;
	case SquareType::Materials: return Resource::Materials;
	}
	Verify("SquareTypeToResource");
	return Resource::_Count;
}

SquareType ResourceToSquareType(Resource t)
{
	switch (t)
	{
	case Resource::Money: return SquareType::Money;
	case Resource::Science: return SquareType::Science;
	case Resource::Materials: return SquareType::Materials;
	}
	Verify("ResourceToSquareType");
	return SquareType::_Count;
}

std::default_random_engine& GetRandom()
{ 
	static std::default_random_engine engine((unsigned int)::time(nullptr) );
	return engine;
}