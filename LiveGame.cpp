#include "stdafx.h"
#include "LiveGame.h"
#include "App.h"
#include "Player.h"
#include "CmdStack.h"
#include "EnumTraits.h"
#include "Players.h"
#include "Record.h"
#include "Serial.h"
#include "ActionPhase.h"
#include "CombatPhase.h"
#include "ChooseTeamPhase.h"
#include "UpkeepPhase.h"
#include "Test.h"

#include <algorithm>

LiveGame::LiveGame() : m_gamePhase(GamePhase::Lobby), m_nextRecordID(1)
{
}

LiveGame::LiveGame(int id, const std::string& name, const Player& owner) : 
Game(id, name, owner), m_gamePhase(GamePhase::Lobby), m_nextRecordID(1)
{
}

LiveGame::~LiveGame()
{
}

void LiveGame::EnjoinPlayer(const Player& player, bool join)
{
	VERIFY_MODEL_MSG(m_name, !HasStarted());
	Team* team = FindTeam(player);
	VERIFY_MODEL(join == !team);

	if (join)
		m_teams.push_back(TeamPtr(new Team(player.GetID())));
	else
		m_teams.erase(std::find_if(m_teams.begin(), m_teams.end(), [&](const TeamPtr& t) { return t.get() == team; } ));
}

void LiveGame::StartChooseTeamGamePhase()
{
	VERIFY_MODEL_MSG(m_name, !HasStarted());
	VERIFY_MODEL_MSG(m_name, !m_teams.empty());
	
	m_gamePhase = GamePhase::ChooseTeam;
	m_pPhase = PhasePtr(new ChooseTeamPhase(this));

	// Decide team order.
	for (size_t i = 0; i < m_teams.size(); ++i)
		m_turnOrder.push_back(i);
	std::shuffle(m_turnOrder.begin(), m_turnOrder.end(), GetRandom());

	//for (int i = 0; i < FakePlayers; ++i)
	//{
	//	std::ostringstream ss;
	//	ss << "Fake " << i;
	//	m_teams.insert(std::make_pair(ss.str(), nullptr));
	//	m_teamOrder.insert(m_teamOrder.begin() + i, ss.str());
	//	AssignTeam(ss.str(), RaceType::Human, Colour(i + 1));
	//}

	// Initialise hex bags.
	for (auto r : EnumRange<HexRing>())
		m_hexBag[(int)r] = HexBag(r, m_teams.size());

	//m_hexBag[1].ReturnTile(206); // Discovery, no ancients, 1 materials square.

	m_repBag.Init();
	m_techBag.Init();
	m_discBag.Init();
}


void LiveGame::StartMainGamePhase()
{
	VERIFY_MODEL(m_gamePhase == GamePhase::ChooseTeam);

	m_gamePhase = GamePhase::Main;
	m_state.Init(*this);
	StartActionPhase();
}

Phase& LiveGame::GetPhase()
{
	VERIFY_MODEL(!!m_pPhase);
	return *m_pPhase;
}

ActionPhase& LiveGame::GetActionPhase()
{
	VERIFY_MODEL(m_pPhase && dynamic_cast<ActionPhase*>(m_pPhase.get()));
	return static_cast<ActionPhase&>(*m_pPhase);
}

ChooseTeamPhase& LiveGame::GetChooseTeamPhase()
{
	VERIFY_MODEL(m_pPhase && dynamic_cast<ChooseTeamPhase*>(m_pPhase.get()));
	return static_cast<ChooseTeamPhase&>(*m_pPhase);
}

UpkeepPhase& LiveGame::GetUpkeepPhase()
{
	VERIFY_MODEL(m_pPhase && dynamic_cast<UpkeepPhase*>(m_pPhase.get()));
	return static_cast<UpkeepPhase&>(*m_pPhase);
}

const CombatPhase& LiveGame::GetCombatPhase() const
{
	VERIFY_MODEL(m_pPhase && dynamic_cast<CombatPhase*>(m_pPhase.get()));
	return static_cast<CombatPhase&>(*m_pPhase);
}

bool LiveGame::NeedCombat() const
{
	return false;
}

void LiveGame::StartActionPhase()
{
	m_pPhase = PhasePtr(new ActionPhase(this));
}

void LiveGame::FinishActionPhase(const std::vector<Colour>& passOrder)
{
	VERIFY_MODEL(passOrder.size() == m_teams.size());
	
#if 0	// Special passing rule.
	m_turnOrder.clear();
	for (Colour c : passOrder)
		for (size_t i = 0; i < m_teams.size(); ++i)
			if (m_teams[i]->GetColour() == c)
			{
				m_turnOrder.push_back(i);
				break;
			}
#else // Normal passing rule.
	for (auto it = m_turnOrder.begin(); it != m_turnOrder.end(); ++it)
		if (m_teams[*it]->GetColour() == passOrder.front())
		{
			std::rotate(m_turnOrder.begin(), it, m_turnOrder.end());
			break;
		}
#endif

	//Test::AddShipsToCentre(*this);

	if (GetMap().HasPendingBattle(*this))
		m_pPhase = PhasePtr(new CombatPhase(this));
	else
		m_pPhase = PhasePtr(new UpkeepPhase(this));
}

void LiveGame::FinishCombatPhase()
{
	m_pPhase = PhasePtr(new UpkeepPhase(this));
}

int LiveGame::PushRecord(std::unique_ptr<Record> pRec)
{
	GameState state(m_state, *this);
	pRec->Undo(*this, nullptr);
	pRec->Do(*this, nullptr);
	VERIFY(m_state == state);

	int id = m_nextRecordID++;
	pRec->SetID(id);
	m_records.push_back(std::move(pRec));
	return id;
}

RecordPtr LiveGame::PopRecord()
{
	size_t pop = GetLastPoppableRecord();

	VERIFY_MODEL(pop >= 0);

	RecordPtr pRec = std::move(m_records[pop]);
	m_records.erase(m_records.begin() + pop);
	return pRec;
}

int LiveGame::GetLastPoppableRecord() const
{
	int lastPoppable = (int)m_records.size() - 1;
	for (auto rec = m_records.rbegin(); rec != m_records.rend() && (*rec)->IsMessageRecord(); ++rec)
		--lastPoppable;

	return lastPoppable;
}

LiveGame::LogVec LiveGame::GetLogs() const
{
	LogVec logs;
	for (auto& rec : m_records)
	{
		std::string msg = rec->GetMessage(*this);
		if (!msg.empty())
			logs.push_back(LogVec::value_type(rec->GetID(), msg));
	}
	return logs;
}

const Team& LiveGame::GetTeamForTurn(int i) const
{
	VERIFY(m_turnOrder.size() == m_teams.size() && i < (int)m_turnOrder.size());
	return *m_teams[m_turnOrder[i]];
}

void LiveGame::Save() const
{
	std::ostringstream ss;
	ss << "data/games/live/" << m_id << ".xml";
	bool bOK = Serial::SaveClass(ss.str(), *this);
	ASSERT(bOK);
}

void LiveGame::Save(Serial::SaveNode& node) const 
{
	node.SaveCntr("records", m_records, Serial::ObjectSaver());
	node.SaveObject("phase", m_pPhase);
	node.SaveEnum("game_phase", m_gamePhase);
	node.SaveType("next_record_id", m_nextRecordID);
	node.SaveCntr("turn_order", m_turnOrder, Serial::TypeSaver());
	__super::Save(node);

	node.SaveClass("state", m_state);
}

void LiveGame::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("records", m_records, Serial::ObjectLoader());
	node.LoadObject("phase", m_pPhase);
	node.LoadEnum("game_phase", m_gamePhase);
	node.LoadType("next_record_id", m_nextRecordID);
	node.LoadCntr("turn_order", m_turnOrder, Serial::TypeLoader());
	__super::Load(node);

	GameState state(*this);
	node.LoadClass("state", state);

	if (m_gamePhase == GamePhase::Main)
	{
		m_state.Init(*this);
		for (auto& r : m_records)
			r->Do(*this, nullptr);

		VERIFY(state == m_state);
	}

	if (m_pPhase)
		m_pPhase->SetGame(*this);
}

DEFINE_ENUM_NAMES(LiveGame::GamePhase) { "Lobby", "ChooseTeam", "Main", "" };
