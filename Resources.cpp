#include "stdafx.h"
#include "Resources.h"
#include "App.h"
#include "EnumTraits.h"
#include "Serial.h"

DEFINE_ENUM_NAMES(Resource) { "Money", "Science", "Materials", "" };

void Resources::operator+=(const Resources& rhs)
{
	for (auto r : EnumRange<Resource>())
		at(r) += rhs.at(r);
}

void Resources::operator-=(const Resources& rhs)
{
	for (auto r : EnumRange<Resource>())
		at(r) -= rhs.at(r);
}

void Resources::Save(Serial::SaveNode& node) const
{
	for (auto r : EnumRange<Resource>())
		node.SaveType(::EnumToString(r), operator[](r));
}

void Resources::Load(const Serial::LoadNode& node)
{
	for (auto r : EnumRange<Resource>())
		node.LoadType(::EnumToString(r), operator[](r));
}

bool Resources::IsOrbitalType(Resource r)
{
	return r == Resource::Money || r == Resource::Materials;
}
