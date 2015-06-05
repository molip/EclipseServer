#include "stdafx.h"
#include "InfluenceTrack.h"

#include "App.h"

int InfluenceTrack::GetUpkeep(int nDiscs)
{
	static const int vals[] = { 30, 25, 21, 17, 13, 10, 7, 5, 3, 2, 1, 0, 0, 0, 0, 0 };

	VERIFY_MODEL(nDiscs >= 0 && nDiscs < _countof(vals));
	return vals[nDiscs];
}
