#include "stdafx.h"
#include "Resources.h"
#include "App.h"
#include "EnumTraits.h"
#include "Serial.h"

DEFINE_ENUM_NAMES(Resource) { "Money", "Science", "Materials", "" };

void Resources::Save(Serial::SaveNode& node) const
{
	for (auto r : EnumRange<Resource>())
		node.SaveType(EnumTraits<Resource>::ToString(r), operator[](r));
}

void Resources::Load(const Serial::LoadNode& node)
{
	for (auto r : EnumRange<Resource>())
		node.LoadType(EnumTraits<Resource>::ToString(r), operator[](r));
}
