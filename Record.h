#pragma once

#include "App.h"
#include "Dynamic.h"

#include <memory>
#include <functional>

class Game;
class Controller;
class LiveGame;
class ReviewGame;

enum class Colour;

class Record;
DEFINE_UNIQUE_PTR(Record);

class Record : public Dynamic
{
public:
	Record();
	virtual ~Record();
	
	static void DoAndPush(RecordPtr pRec, const LiveGame& game, const Controller& controller);
	static RecordPtr PopAndUndo(const LiveGame& game, const Controller& controller);
	static void Do(RecordPtr pRec, const LiveGame& game, const Controller& controller);
	static void DoImmediate(const LiveGame& game, const std::function<void(LiveGame&)>& fn);
	static void DoImmediate(const ReviewGame& game, const std::function<void(ReviewGame&)>& fn);

	void Do(const ReviewGame& game, const Controller& controller);
	void Undo(const ReviewGame& game, const Controller& controller);
	
	virtual bool WantMergeNext() const { return false; }

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) = 0;

};

class TeamRecord : public Record
{
public:
	TeamRecord();
	TeamRecord(Colour colour);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;
protected:
	Colour m_colour;
};

class GameRecord : public Record
{
public:
	GameRecord(const std::function<void(Game&)>& fn) : m_fn(fn) {}
private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) override;

	const std::function<void(Game&)> m_fn;
};
