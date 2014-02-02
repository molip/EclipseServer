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
#include "ChooseTeamPhase.h"

#include <algorithm>

LiveGame::LiveGame() : m_gamePhase(GamePhase::Lobby), m_iRound(-1)
{
}

LiveGame::LiveGame(int id, const std::string& name, const Player& owner) : 
	Game(id, name, owner), m_gamePhase(GamePhase::Lobby), m_iRound(-1)
{
}

LiveGame::~LiveGame()
{
}

void LiveGame::AddPlayer(Player& player)
{
	AssertThrow("LiveGame::AddTeam: Game already started: " + m_name, !HasStarted());
	if (!FindTeam(player))
		m_teams.push_back(TeamPtr(new Team(m_id, player.GetID())));
	Save();
}

void LiveGame::StartChooseTeamGamePhase()
{
	AssertThrow("LiveGame::Start: Game already started: " + m_name, !HasStarted());
	AssertThrow("LiveGame::Start: Game has no players: " + m_name, !m_teams.empty());
	
	m_gamePhase = GamePhase::ChooseTeam;
	m_pPhase = PhasePtr(new ChooseTeamPhase);
	m_pPhase->SetGame(*this);

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

	Save();
}

void LiveGame::StartMainGamePhase()
{
	AssertThrowModel("LiveGame::StartMainGamePhase", m_gamePhase == GamePhase::ChooseTeam);

	m_gamePhase = GamePhase::Main;
	
	m_pPhase = PhasePtr(new ActionPhase);
	m_pPhase->SetGame(*this);

	Hex& centre = m_map.AddHex(MapPos(0, 0), 001, 0);
	centre.AddShip(ShipType::GCDS, Colour::None);

	// Initialise starting hexes.
	auto startPositions = m_map.GetTeamStartPositions();
	assert(startPositions.size() == m_teams.size());
	for (size_t i = 0; i < m_teams.size(); ++i)
	{
		Team& team = *m_teams[i];
		Race r(team.GetRace());
		int idHex = r.GetStartSector(team.GetColour());

		Hex& hex = m_map.AddHex(startPositions[i], idHex, 0);

		team.PopulateStartHex(hex);
	}
	StartRound();

	Save();
}

Phase& LiveGame::GetPhase()
{
	AssertThrow("LiveGame::GetPhase", !!m_pPhase);
	return *m_pPhase;
}

ActionPhase& LiveGame::GetActionPhase()
{
	AssertThrow("LiveGame::GetActionPhase", m_pPhase && dynamic_cast<ActionPhase*>(m_pPhase.get()));
	return static_cast<ActionPhase&>(*m_pPhase);
}

ChooseTeamPhase& LiveGame::GetChooseTeamPhase()
{
	AssertThrow("LiveGame::GetChooseTeamPhase", m_pPhase && dynamic_cast<ChooseTeamPhase*>(m_pPhase.get()));
	return static_cast<ChooseTeamPhase&>(*m_pPhase);
}

bool LiveGame::NeedCombat() const
{
	return false;
}

void LiveGame::StartRound()
{
	assert(m_iRound < 9);

	++m_iRound;

	if (m_iRound == 9) // Game over.
		return;

	// Take new technologies from bag.
	const int startTech[] = { 12, 12, 14, 16, 18, 20 };
	const int roundTech[] = { 4, 4, 6, 7, 8, 9 };

	int nTech = (m_iRound == 0 ? startTech : roundTech)[m_teams.size() - 1];
	for (int i = 0; i < nTech && !m_techBag.IsEmpty(); ++i)
		++m_techs[m_techBag.TakeTile()];
}

void LiveGame::PushRecord(std::unique_ptr<Record>& pRec)
{
	m_records.push_back(std::move(pRec));
	Save();
}

RecordPtr LiveGame::PopRecord()
{
	AssertThrow("LiveGame::PopRecord", !m_records.empty());
	RecordPtr pRec = std::move(m_records.back());
	m_records.pop_back();
	Save();
	return pRec;
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
	__super::Save(node);
	node.SaveEnum("game_phase", m_gamePhase);
	node.SaveCntr("records", m_records, Serial::ObjectSaver());
	node.SaveType("round", m_iRound);
	node.SaveObject("phase", m_pPhase);
}

void LiveGame::Load(const Serial::LoadNode& node)
{
	__super::Load(node);
	node.LoadEnum("game_phase", m_gamePhase);
	node.LoadCntr("records", m_records, Serial::ObjectLoader());
	node.LoadType("round", m_iRound);
	node.LoadObject("phase", m_pPhase);

	if (m_pPhase)
		m_pPhase->SetGame(*this);
}

DEFINE_ENUM_NAMES(LiveGame::GamePhase) { "Lobby", "ChooseTeam", "Main", "" };
