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
	
	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	static void DoImmediate(const ReviewGame& game, const std::function<void(ReviewGame&)>& fn);

	void Do(const ReviewGame& game, const Controller& controller);
	void Undo(const ReviewGame& game, const Controller& controller);

	void Do(LiveGame& game, const Controller& controller);
	void Undo(LiveGame& game, const Controller& controller);

	void SetID(int id) { m_id = id; }
	int GetID() const { return m_id; }

	virtual bool IsMessageRecord() const { return false; }

	virtual std::string GetMessage(const Game& game) const { return std::string(); }

private:
	virtual void Apply(bool bDo, Game& game, const Controller& controller) = 0;

	int m_id;
};

class TeamRecord : public Record
{
public:
	TeamRecord();
	TeamRecord(Colour colour);

	virtual void Save(Serial::SaveNode& node) const override;
	virtual void Load(const Serial::LoadNode& node) override;

	virtual std::string GetMessage(const Game& game) const override;

protected:
	virtual std::string GetTeamMessage() const { return std::string(); }
	
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
