#pragma once

#include "App.h"
#include "Dynamic.h"
#include "GameStateAccess.h"

#include <memory>
#include <functional>

class Game;
class Controller;
class LiveGame;
class ReviewGame;
class Team;
class TeamState;

enum class Colour;

class Record;
DEFINE_UNIQUE_PTR(Record);

namespace Output { class Message; }
class Player;

class RecordContext : public GameStateAccess
{
public:
	RecordContext(Game& game, const Controller* controller);
	void SendMessage(const Output::Message& msg, const Player* pPlayer = nullptr) const;
	GameState& GetGameState() const { return __super::GetGameState(m_game); }
	const Game& GetGame() const { return m_game; }
private:
	Game& m_game;
	const Controller* m_controller;
};

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

	void Do(LiveGame& game, const Controller* controller);
	void Undo(LiveGame& game, const Controller* controller);

	void SetID(int id) { m_id = id; }
	int GetID() const { return m_id; }

	virtual bool IsMessageRecord() const { return false; }

	virtual std::string GetMessage(const Game& game) const = 0;

private:
	virtual void Apply(bool bDo, const Game& game, GameState& gameState) = 0;
	virtual void Update(const Game& game, const RecordContext& context) const {}

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
	virtual std::string GetTeamMessage() const { return ""; }
	
	virtual void Apply(bool bDo, const Game& game, GameState& gameState) override;
	virtual void Update(const Game& game, const RecordContext& context) const override;
	virtual void Apply(bool bDo, const Game& game, const Team& team, GameState& gameState, TeamState& teamState) = 0;
	virtual void Update(const Game& game, const Team& team, const RecordContext& context) const {}

	Colour m_colour;
};

class GameRecord : public Record
{
public:
	GameRecord(const std::function<void(Game&)>& fn) : m_fn(fn) {}
private:
	virtual void Apply(bool bDo, const Game& game, GameState& gameState) override;

	const std::function<void(Game&)> m_fn;
};
