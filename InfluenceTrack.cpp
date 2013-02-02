#include "stdafx.h"
#include "InfluenceTrack.h"
#include <algorithm>

int InfluenceTrack::GetUpkeep(int nDiscs)
{
	static const int vals[] = { 30, 25, 21, 17, 13, 10, 7, 5, 3, 2, 1 };

	return nDiscs >= 0 && nDiscs < _countof(vals) ? vals[nDiscs] : 0;
}

//0  1  2  3  4  5  6  7  8  9 10 11
//0  1  2  3  5  7 10 13 17 21 25 30 

//0  0  0  0  1  2  4  6  9  12  15  19
       
