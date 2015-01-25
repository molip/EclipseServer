
function OnCommand(elem)
{
	var type = elem.type

	if (type == 'show')
		OnCommandShow(elem)
	else if (type == 'update')
		OnCommandUpdate(elem)
	else if (type == 'choose')
		OnCommandChoose(elem)
	else
		alert('OnCommand: unknown command: ' + type)
}

function OnCommandShow(elem)
{
	var panel = elem.panel

	var panels =['game_list_panel', 'lobby_panel', 'choose_panel', 'game_panel']
    var found = false
    for (var i = 0; i < panels.length; ++i)
	{
		var div = document.getElementById(panels[i])
		if (panel == panels[i])
		{
		    div.style.display = "block"
		    found = true
		}
		else
		{
		    div.style.display = "none" 
		} 
	}
    if (!found)
        alert('OnCommandShow: unknown element: ' + panel)
		
	if (panel == 'choose_panel')
	{
		ShowElementById('choose_team', false)
	}
	else if (panel == 'game_panel')
	{
		Map.Init()
		Combat.Init()

		ShowElementById('choose_subaction', false)
		ShowElementById('choose_undo', false)
		ShowActionElement(null)
		
		ShowElementById('choose_upkeep', false)

		document.getElementById('output').innerText = ''
	}
}

function OnCommandUpdate(elem)
{
	var param = elem.param

	if (param == "game_list")
		OnCommandUpdateGameList(elem)
	else if (param == "lobby")
		OnCommandUpdateLobby(elem)
	else if (param == "lobby_controls")
		OnCommandUpdateLobbyControls(elem)
	else if (param == "choose_team")
		OnCommandUpdateChooseTeam(elem)
	else if (param == "teams")
		OnCommandUpdateTeams(elem)
	else if (param == "team")
		OnCommandUpdateTeam(elem)
	else if (param == "influence_track")
		OnCommandUpdateInfluenceTrack(elem)
	else if (param == "action_track")
		OnCommandUpdateActionTrack(elem)
	else if (param == "colony_ships")
		OnCommandUpdateColonyShips(elem)
	else if (param == "storage_track")
		OnCommandUpdateStorageTrack(elem)
	else if (param == "technology_track")
		OnCommandUpdateTechnologyTrack(elem)
	else if (param == "population_track")
		OnCommandUpdatePopulationTrack(elem)
	else if (param == "reputation_track")
		OnCommandUpdateReputationTrack(elem)
	else if (param == "victory_point_tiles")
		OnCommandUpdateVictoryTiles(elem)
	else if (param == "passed")
		OnCommandUpdatePassed(elem)
	else if (param == "map")
		OnCommandUpdateMap(elem)
	else if (param == "review_ui")
		OnCommandUpdateReviewUI(elem)
	else if (param == "technologies")
		OnCommandUpdateTechnologies(elem)
	else if (param == "round")
		OnCommandUpdateRound(elem)
	else if (param == "blueprints")
		OnCommandUpdateBlueprints(elem)
	else if (param == "show_combat")
		OnCommandUpdateShowCombat(elem)
	else if (param == "combat")
		OnCommandUpdateCombat(elem)
	else if (param == "add_log")
		OnCommandAddLog(elem)
	else if (param == "remove_log")
		OnCommandRemoveLog(elem)
	else
        alert('OnCommandUpdate: unknown param: ' + param)
}

function OnCommandChoose(elem)
{
	var param = elem.param

	if (param == "team")
		OnCommandChooseTeam(elem)
	else if (param == "finished")
		OnCommandChooseFinished(elem)
	else if (param == "upkeep")
		OnCommandChooseUpkeep(elem)
	else if (param == "action")
		OnCommandChooseAction(elem)
	else if (param == "explore_pos")
		Explore.OnCommandChoosePos(elem)
	else if (param == "explore_hex")
		Explore.OnCommandChooseHex(elem)
	else if (param == "discovery")
		Discovery.OnCommandChooseDiscovery(elem)
	else if (param == "influence_src")
		Influence.OnCommandChooseSrc(elem)
	else if (param == "influence_dst")
		Influence.OnCommandChooseDst(elem)
	else if (param == "colonise_pos")
		Colonise.OnCommandChoosePos(elem)
	else if (param == "colonise_squares")
		Colonise.OnCommandChooseSquares(elem)
	else if (param == "research")
		Research.OnCommand(elem)
	else if (param == "research_artifact")
		Research.OnCommandArtifact(elem)
	else if (param == "build")
		Build.OnCommand(elem)
	else if (param == "upgrade")
		Upgrade.OnCommand(elem)
	else if (param == "move_src")
		Move.OnCommandChooseSrc(elem)
	else if (param == "move_dst")
		Move.OnCommandChooseDst(elem)
	else if (param == "diplomacy")
		Diplomacy.OnCommand(elem)
	else if (param == "trade")
		Trade.OnCommand(elem)
	else if (param == "combat")
		Combat.OnCommand(elem)
	else if (param == "dice")
		Combat.OnDiceCommand(elem)
	else if (param == "uncolonise")
		Uncolonise.OnCommand(elem)
	else
		alert('OnCommandChoose: unknown param: ' + param)

	if (data.action)
	{
		ShowElementById('choose_undo', true)
		ShowElementById('choose_subaction', !data.action.flagNoSubAction)
		document.getElementById('choose_undo_btn').disabled = data.action.flagCantUndo
	}
}

function OnCommandUpdateGameList(elem)
{
	Assert(elem.games)
	
	var game_str = '<a href="Join Game" onclick="SendJoinGame({0});return false;">{1}</a>{2}(<b> {3}</b>, {4})<br/>'
	
	var html = ''

	for (var i in elem.games)
	{
		var game = elem.games[i]
		Assert(game.players && game.id && game.name && game.owner)
		var players = ''
		for (var j in game.players)
			players += game.players[j] + ','

		html += game_str.format(game.id, game.name, game.started ? '[started]' : '', game.owner, players)
	}
	
	html += '<br/><button type="button" onclick="SendCreateGame()">Create Game</button>'

	document.getElementById('game_list_content').innerHTML = html
}

function OnCommandUpdateLobby(elem)
{		
	Assert(elem.game && elem.players && elem.owner)

	var html = '<h2>{0}</h2><br/><b>{1}</b>,'.format(elem.game, elem.owner)
	for (var i = 0, player;  player = elem.players[i]; ++i)
		html += player + ','

	document.getElementById('lobby_content').innerHTML = html
}

function OnCommandUpdateLobbyControls(elem)
{	
	Assert(elem.show != null)

	var html = elem.show ? '<br/><br/><button type="button" onclick="SendStartGame()">Start Game</button>' : ''
	document.getElementById('lobby_owner_controls').innerHTML = html
}

function OnCommandUpdateChooseTeam(elem)
{		
	Assert(elem.game && elem.teams)

	var html = '<h2>{0}</h2><br/>'.format(elem.game)

	for (var i = 0, team; team = elem.teams[i]; ++i)
	{
		Assert(team.name)
		html += '{0}:{1}:{2}<br/>'.format(team.name, team.race || '', team.colour || '')
	}
	document.getElementById('choose_team_content').innerHTML = html
}

function OnCommandUpdateTeams(elem)
{		
	data.teams = {}
	
	var fmt_tab = '<button type="button" onclick="ShowTeamPage(\'{0}\', true)" style="{2}">{1}</button>'
	var html_tabs = ''

	for (var i = 0, team; team = elem.teams[i]; ++i)
	{
		html_tabs += fmt_tab.format(team.id, team.name, team.id == data.playerID ? 'font-weight: bold' : '')

		data.teams[team.id] = {}
		data.teams[team.id].population = {}
		data.teams[team.id].blueprint_type = team.blueprints
	}

	Blueprints.Init()
	Population.Init()

	html_tabs +='	<button type="button" onclick="ShowSupplyPage()">Supply</button>\
					<div style="float:right">\
						<button type="button" onclick="ShowTeamGeneral()">General</button>\
						<button type="button" onclick="ShowTeamBlueprints()">Blueprints</button>\
					</div>'
					
	document.getElementById('game_tabs').innerHTML = html_tabs
	
	ShowTeamPage(data.playerID, false) // Don't update yet - team data hasn't been set. 
	ShowTeamGeneral()

	ShowElementById('live_ui', elem.game_type == "live")
	ShowElementById('review_ui', elem.game_type == "review")
	
	ShowCombat(false)
}

function OnCommandUpdateTeam(elem)
{	
	var team = data.teams[elem.id]
	team.name = elem.name
	team.race = elem.race
	team.colour = elem.colour
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateSummary()
}

function OnCommandUpdatePassed(elem)
{
	var team = data.teams[elem.id]
	team.has_passed = elem.has_passed
	
	if (IsCurrentTeam(elem.id))
		Team.UpdatePassed()
}

function OnCommandUpdateInfluenceTrack(elem)
{
	var team = data.teams[elem.id]
	team.discs = elem.discs
	team.upkeep = elem.upkeep
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateInfluence()
}

function OnCommandUpdateActionTrack(elem)
{
	var team = data.teams[elem.id]
	team.actions = elem.discs
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateActions()
}

function OnCommandUpdateColonyShips(elem)
{
	var team = data.teams[elem.id]
	team.colony_ships_used = elem.used
	team.colony_ships_total = elem.total
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateColonyShips()
}

function OnCommandUpdateStorageTrack(elem)
{
	var team = data.teams[elem.id]
	team.Money = elem.Money
	team.Science = elem.Science
	team.Materials = elem.Materials
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateStorage()
}

function OnCommandUpdateTechnologyTrack(elem)
{
	var team = data.teams[elem.id]
	team.classes = elem.classes
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateTechnology()
}

function OnCommandUpdatePopulationTrack(elem)
{
	var team = data.teams[elem.id]
	team.population.Money = elem.Money
	team.population.Science = elem.Science
	team.population.Materials = elem.Materials
	
	if (IsCurrentTeam(elem.id))
		Team.UpdatePopulation()
}

function OnCommandUpdateReputationTrack(elem)
{
	var team = data.teams[elem.id]
	team.tiles = elem.tiles
	team.slots = elem.slots
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateReputation()
}

function OnCommandUpdateVictoryTiles(elem)
{
	var team = data.teams[elem.id]
	team.victory_tiles = elem.victory_tiles
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateVictoryTiles()
}

function OnCommandUpdateMap(elem)
{
	Map.Clear()
	
	for (var i = 0, hex; hex = elem.hexes[i]; ++i)
		Map.AddHex(hex.id, new Point(hex.x, hex.y), hex.rotation, hex.colour, hex.squares, hex.ships)

	Map.Draw()
}

function OnCommandUpdateReviewUI(elem)
{
	document.getElementById('retreat_review').disabled = !elem.can_retreat
	document.getElementById('advance_review').disabled = !elem.can_advance
	
	var next_record_id = elem.next_record_id ? 'log_item_{0}'.format(elem.next_record_id) : null

	var div = document.getElementById('output')
	var colour = "black"
	for (var e = div.firstChild; e; e = e.nextSibling)
	{
		if (e.id == next_record_id)
			colour = "gray"
		e.style.color = colour
	}
}

function OnCommandUpdateTechnologies(elem)
{
	var html = ''
	
	for (var j = 0, tech; tech = elem.techs[j]; ++j)
		html += '{0} x {1} ({2}/{3})<br/>'.format(tech.count, tech.type, tech.max_cost, tech.min_cost)

	document.getElementById('supply_technologies').innerHTML = html
}

function OnCommandUpdateRound(elem)
{
	document.getElementById('round_count').innerHTML = elem.round
}

function OnCommandUpdateBlueprints(elem)
{
	var team = data.teams[elem.id]
	team.blueprints = elem.blueprints
	
	if (IsCurrentTeam(elem.id))
		Team.UpdateBlueprints()
}

function OnCommandUpdateShowCombat(elem)
{
	ShowCombat(elem.show)
}

function OnCommandUpdateCombat(elem)
{
	Combat.Update(elem.invader, elem.defender)
}

function OnCommandAddLog(elem)
{
	var colour = IsElementVisible('review_ui') ? 'gray' : 'black'

	var div = document.getElementById('output')
	for (var i = 0, item; item = elem.items[i]; ++i)
	{
		var msg = item.message.replace('\n', '<br>')
		var html = '<div id="log_item_{0}" style="color:{1}">'.format(item.id, item.id == 0 ? 'red' : colour) + msg + '<br></div>'
		div.innerHTML += html
	}
	
	div.scrollTop = div.scrollHeight;
}

function OnCommandRemoveLog(elem)
{
	var span = document.getElementById('log_item_{0}'.format(elem.id))
	if (span)
		if (IsElementVisible('review_ui'))
			span.parentNode.removeChild(span)
		else
			span.style.textDecoration = 'line-through'
}

function OnCommandChooseTeam(elem)
{		
	Assert(elem.active != null)
	
	var html = ''
	if (elem.active)
	{
		html = '<div>Race: <select id="select_race">'
		
		for (var i = 0, team; team = elem.teams[i]; ++i)
			html += '<option value="{0},{1}">{0} ({1})</option>'.format(team.name, team.colour)

		html += '</select><button type="button" onclick="SendChooseTeam()">OK</button></div>'
	}
	var div = document.getElementById('choose_team')
	div.innerHTML = html
	ShowElement(div, elem.active)
}

function OnCommandChooseAction(elem)
{
	document.getElementById('choose_undo_btn').disabled = !elem.can_undo
	document.getElementById('choose_action_explore_btn').disabled = !elem.can_explore
	document.getElementById('choose_action_influence_btn').disabled = !elem.can_influence
	document.getElementById('choose_action_research_btn').disabled = !elem.can_research
	document.getElementById('choose_action_upgrade_btn').disabled = !elem.can_upgrade
	document.getElementById('choose_action_build_btn').disabled = !elem.can_build
	document.getElementById('choose_action_move_btn').disabled = !elem.can_move
	document.getElementById('choose_action_colonise_btn').disabled = !elem.can_colonise
	document.getElementById('choose_action_diplomacy_btn').disabled = !elem.can_diplomacy
	document.getElementById('choose_action_trade_btn').disabled = !elem.can_trade
	document.getElementById('choose_action_pass_btn').disabled = !elem.can_pass
	document.getElementById('choose_end_turn_btn').disabled = !elem.can_end_turn

	ShowElementById('choose_subaction', true)
	ShowElementById('choose_action_diplomacy_btn', true, true)
	ShowElementById('choose_action_bankrupt_btn', false, true)

	ShowElementById('choose_undo', true)
	ShowActionElement('choose_action')
}

function OnCommandChooseFinished(elem)
{
	ShowElementById('choose_subaction', false)
	ShowElementById('choose_undo', false)
	ShowActionElement(null)
}

function OnCommandChooseUpkeep(elem)
{
	ShowActionElement('choose_upkeep') 
	ShowElementById('choose_undo', true)

	ShowElementById('choose_subaction', true)
	ShowElementById('choose_action_diplomacy_btn', false, true)
	ShowElementById('choose_action_bankrupt_btn', true, true)

	document.getElementById('choose_action_colonise_btn').disabled = !elem.can_colonise
	document.getElementById('choose_action_trade_btn').disabled = !elem.can_trade
	document.getElementById('choose_action_bankrupt_btn').disabled = !elem.can_bankrupt
	document.getElementById('choose_undo_btn').disabled = !elem.can_undo
	
	ExitAction()
}
