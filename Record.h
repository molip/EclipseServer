#pragma once

#include "App.h"
#include "Dynamic.h"

#include <memory>

class Game;
class Controller;
enum class Colour;

class Record : public Dynamic
{
public:
	Record();
	Record(Colour colour);
	virtual ~Record();
	virtual void Do(Game& game, const Controller& controller)	{ Apply(true, game, controller); }
	virtual void Undo(Game& game, const Controller& controller)	{ Apply(false, game, controller); }
	virtual void Save(Serial::SaveNode& node) const override; 
	virtual void Load(const Serial::LoadNode& node) override; 
protected:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) { ASSERT(false); }

	Colour m_colour;
};

typedef std::unique_ptr<Record> RecordPtr;