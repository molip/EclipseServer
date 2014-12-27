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

void LiveGame::AddPlayer(const Player& player)
{
	VERIFY_MODEL_MSG(m_name, !HasStarted());
	if (!FindTeam(player))
		m_teams.push_back(TeamPtr(new Team(player.GetID())));
}

void LiveGame::StartChooseTeamGamePhase()
{
	VERIFY_MODEL_MSG(m_name, !HasStarted());
	VERIFY_MODEL_MSG(m_name, !m_teams.empty());
	
	m_gamePhase = GamePhase::ChooseTeam;
	m_pPhase = PhasePtr(new ChooseTeamPhase(this));

	// Decide team order.
	std::shuffle(m_teams.begin(), m_teams.end(), GetRandom());

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

void LiveGame::AssignTeam(Player& player, RaceType race, Colour colour)
{
	auto it = m_state.m_teamStates.insert(std::make_pair(colour, TeamStatePtr(new TeamState)));
	GetTeam(player).Assign(race, colour, *it.first->second, *this);
}

void LiveGame::StartMainGamePhase()
{
	VERIFY_MODEL(m_gamePhase == GamePhase::ChooseTeam);

	m_gamePhase = GamePhase::Main;
	
	Map& map = GetMap();
	Hex& centre = map.AddHex(MapPos(0, 0), 001, 0);
	centre.AddShip(ShipType::GCDS, Colour::None);

	// Initialise starting hexes.
	auto startPositions = map.GetTeamStartPositions();
	assert(startPositions.size() == m_teams.size());
	for (size_t i = 0; i < m_teams.size(); ++i)
	{
		Team& team = *m_teams[i];
		Race r(team.GetRace());
		int idHex = r.GetStartSector(team.GetColour());

		Hex& hex = map.AddHex(startPositions[i], idHex, 0);

		team.PopulateStartHex(hex);
	}
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
	std::map<Colour, TeamPtr> map;
	for (auto& team : m_teams)
		map[team->GetColour()] = std::move(team);

	m_teams.clear();
	for (Colour c : passOrder)
		m_teams.push_back(std::move(map[c]));
#else // Normal passing rule.
	for (auto it = m_teams.begin(); it != m_teams.end(); ++it)
		if ((*it)->GetColour() == passOrder.front())
		{
			std::rotate(m_teams.begin(), it, m_teams.end());
			break;
		}
#endif

	Test::AddShipsToCentre(*this);

	if (GetMap().FindPendingBattleHex(*this))
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

void LiveGame::Verify()
{
	GameState state(m_state, *this);
	VERIFY(state == m_state);

	for (auto& r : Reverse(m_records))
		r->Undo(*this, nullptr);

	for (auto& r : m_records)
		r->Do(*this, nullptr);

	VERIFY(state == m_state);
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
	__super::Save(node);
}

void LiveGame::Load(const Serial::LoadNode& node)
{
	node.LoadCntr("records", m_records, Serial::ObjectLoader());
	node.LoadObject("phase", m_pPhase);
	node.LoadEnum("game_phase", m_gamePhase);
	node.LoadType("next_record_id", m_nextRecordID);
	__super::Load(node);

	if (m_pPhase)
		m_pPhase->SetGame(*this);
}

DEFINE_ENUM_NAMES(LiveGame::GamePhase) { "Lobby", "ChooseTeam", "Main", "" };
