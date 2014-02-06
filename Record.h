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
	
	void Do(Game& game, const Controller& controller)	{ Apply(true, game, controller); }
	void Undo(Game& game, const Controller& controller)	{ Apply(false, game, controller); }
	
	virtual void Save(Serial::SaveNode& node) const override; 
	virtual void Load(const Serial::LoadNode& node) override; 

	virtual bool WantMergeNext() const { return false; }

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) = 0;

protected:
	Colour m_colour;
};
