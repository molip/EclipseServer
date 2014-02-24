#pragma once

#include "Record.h"

class ActionRecord : public TeamRecord
{
public:
	ActionRecord();
	ActionRecord(Colour colour);
	
	virtual bool WantMergeNext() const { return true; }

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller);
};
