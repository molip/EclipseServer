#pragma once 

#include "EnumArray.h"

enum class HexRing { None = -1, Inner, Middle, Outer, _Count };
enum class SquareType { Money, Science, Materials, Any, Orbital, _Count };

typedef EnumIntArray<SquareType> SquareCounts;