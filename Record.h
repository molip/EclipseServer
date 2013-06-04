#pragma once

#include "App.h"

#include <memory>

class Game;
class Controller;

class Record
{
public:
	virtual ~Record();
	virtual void Do(Game& game, const Controller& controller)	{ Apply(true, game, controller); }
	virtual void Undo(Game& game, const Controller& controller)	{ Apply(false, game, controller); }
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) { ASSERT(false); }
};

typedef std::unique_ptr<Record> RecordPtr;