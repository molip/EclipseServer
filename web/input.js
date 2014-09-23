function SetTeamDivHTML(team_id, elem_suffix, html)
{
	document.getElementById(GetTeamDivIDFromName(team_id, elem_suffix)).innerHTML = html
}

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
		Explore.OnCommandChooseDiscovery(elem)
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
	data.team_count = 0
	data.team_pages = {}
	
	var fmt_tab = '<button type="button" onclick="ShowTeamPage(\'{0}\')">{1}</button>'
	var fmt_page = '\
					<div id="{0}">\
						<div id="{0}_general">\
							<div id="{0}_summary"></div>\
							<div id="{0}_storage"></div>\
							<div id="{0}_population"></div>\
							<div id="{0}_technology"></div>\
							<div id="{0}_reputation"></div>\
							<div id="{0}_influence" onclick="if (data.action && data.action.OnClickInfluenceTrack) data.action.OnClickInfluenceTrack()"></div>\
							<div id="{0}_actions"></div>\
							<div id="{0}_colony_ships"></div>\
							<div id="{0}_passed"></div>\
						</div>\
						<div id="{0}_blueprints" style="display:none; position:relative; overflow:auto">\
							<img src="images/blueprints/{1}.png" height="320">\
							<div id="{0}_blueprints_overlay" style="position:absolute; left:0px; top:0px; width:378px; height:320px">\
							</div>\
						</div>\
					</div>'
	
//								<img src="images/ship_parts/gluon computer.png" style="position:absolute; left:138px; top:76px">
	var html_tabs = '', html_pages = ''

	for (var i = 0, team; team = elem.teams[i]; ++i)
	{
		html_tabs += fmt_tab.format(team.id, team.name)
		html_pages += fmt_page.format(GetTeamPageIDFromIndex(data.team_count), team.blueprints)

		data.team_pages[team.id] = data.team_count++
	}

	html_tabs +='	<button type="button" onclick="ShowSupplyPage()">Supply</button>\
					<div style="float:right">\
						<button type="button" onclick="ShowTeamGeneral()">General</button>\
						<button type="button" onclick="ShowTeamBlueprints()">Blueprints</button>\
					</div>'
					
	document.getElementById('game_tabs').innerHTML = html_tabs
	document.getElementById('game_pages').innerHTML = html_pages
	
	for (var i = 0, team; team = elem.teams[i]; ++i)
		Blueprints.Init(team.id)

	ShowTeamPage(data.playerID)
	ShowTeamGeneral()

	ShowElementById('live_ui', elem.game_type == "live")
	ShowElementById('review_ui', elem.game_type == "review")
	
	ShowCombat(false)
}

function OnCommandUpdateTeam(elem)
{		
	var html = '<b>Team:</b> {0}, <b>Race:</b> {1}, <b>Colour:</b> {2}<br/><br/>'.format(elem.name, elem.race, elem.colour)
	SetTeamDivHTML(elem.id, 'summary', html)
}

function OnCommandUpdatePassed(elem)
{
	var html = '<b>Passed:</b> {0}<br/>'.format(elem.has_passed)
	SetTeamDivHTML(elem.id, 'passed', html)
}

function OnCommandUpdateInfluenceTrack(elem)
{
	var html = '<b>Influence discs:</b> {0}<br/>'.format(elem.discs)
	SetTeamDivHTML(elem.id, 'influence', html)
}

function OnCommandUpdateActionTrack(elem)
{
	var html = '<b>Actions done:</b> {0}<br/>'.format(elem.discs)
	SetTeamDivHTML(elem.id, 'actions', html)
}

function OnCommandUpdateColonyShips(elem)
{
	var html = '<b>Colony ships left:</b> {0}/{1}<br/>'.format(elem.used, elem.total)
	SetTeamDivHTML(elem.id, 'colony_ships', html)
}

function OnCommandUpdateStorageTrack(elem)
{
	var html = '<b>Storage:</b>Money: {0} Science: {1} Materials:{2}<br/>'.format(elem.Money, elem.Science, elem.Materials)
	SetTeamDivHTML(elem.id, 'storage', html)
}

function OnCommandUpdateTechnologyTrack(elem)
{
	var html = '<b>Technology:</b><br/>'
	
	for (var i = 0, cls; cls = elem.classes[i]; ++i)
	{
		html += cls.name + ':'
		for (var j = 0, tech; tech = cls.techs[j]; ++j)
			html += tech.name + ','
		html += '<br/>'
	}
	SetTeamDivHTML(elem.id, 'technology', html)
}

function OnCommandUpdatePopulationTrack(elem)
{
	var html = '<b>Population:</b>Money: {0} Science: {1} Materials:{2}<br/>'.format(elem.Money, elem.Science, elem.Materials)
	SetTeamDivHTML(elem.id, 'population', html)
}

function OnCommandUpdateReputationTrack(elem)
{
	var html = '<b>Reputation:</b>Tiles: {0}/{1}<br/>'.format(elem.tiles, elem.slots)
	SetTeamDivHTML(elem.id, 'reputation', html)
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
	Assert(elem.blueprints.length == 4)

	for (var ship = 0; ship < 4; ++ship)
		for (var slot = 0, part; part = elem.blueprints[ship][slot]; ++slot)
			Blueprints.SetPart(elem.id, ship, slot, part)
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
		var html = '<span id="log_item_{0}" style="color:{1}">'.format(item.id, item.id == 0 ? 'red' : colour) + item.message + '<br></span>'
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
}
