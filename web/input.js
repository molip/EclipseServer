function ClearDiv(div)
{
	while (div.firstChild) 
		div.removeChild(div.firstChild);
}

function SetDivFromXML(div, elem, xsl)
{		
	ClearDiv(div);
	if (xsl == '')
		return;

	var xsl2 = '<?xml version="1.0"?><xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">'
	xsl2 += xsl + '</xsl:stylesheet>'
	
	xslNode = parser.parseFromString(xsl2, "text/xml");
  
	xsltProcessor = new XSLTProcessor()
	xsltProcessor.importStylesheet(xslNode)
	var htmlNode = xsltProcessor.transformToFragment(elem, document)

	div.appendChild(htmlNode)
}

function SetDivFromCommandElem(div, elem, xsl)
{		
	if (xsl == '')
		ClearDiv(div);
	else
		SetDivFromXML(div, elem, '<xsl:template match="/command">' + xsl + '</xsl:template>')
}

function OnCommand(elem)
{
	var type = elem.getAttribute('type')

	if (type == 'show')
		OnCommandShow(elem)
	else if (type == 'update')
		OnCommandUpdate(elem)
	else if (type == 'choose')
		OnCommandChoose(elem)
	else
		writeToScreen('OnCommand: unknown command: ' + type)
}

function OnCommandShow(elem)
{
	var panel = elem.getAttribute('panel')

	var panels =['game_list_panel', 'lobby_panel', 'choose_panel', 'game_panel']
    var found = false
    for (var i = 0; i < panels.length; ++i)
	{
		var div = document.getElementById(panels[i])
		if (panel == panels[i])
		{
			//writeToScreen('OnCmdShow: showing ' + panels[i])
		    div.style.display = "block"
		    found = true
		}
		else
		{
			//writeToScreen('OnCmdShow: hiding ' + panels[i])
		    div.style.display = "none" 
		} 
	}
    if (!found)
        writeToScreen('OnCommandShow: unknown element: ' + panel)
		
	if (panel == 'game_panel')
	{
		Map.Init()
	}
}

function OnCommandUpdate(elem)
{
	var param = elem.getAttribute('param')

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
	else
        writeToScreen('OnCommandUpdate: unknown param: ' + param)
}

function OnCommandChoose(elem)
{
	var param = elem.getAttribute('param')
	var active = IsTrue(elem.getAttribute('active'))

	if (param == "team")
		OnCommandChooseTeam(elem, active)
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
	else if (param == "finished")
		OnCommandChooseFinished(elem)
	else
		writeToScreen('OnCommandChoose: unknown param: ' + param)

	if (data.action)
	{
		ShowElementById('choose_undo', true)
		ShowElementById('choose_subaction', !data.action.flagNoSubAction)
		document.getElementById('choose_undo_btn').disabled = data.action.flagCantUndo
	}
}

function OnCommandUpdateGameList(elem)
{
	var xsl = '\
		<xsl:for-each select="game">\
			<a href="Join Game" onclick="SendJoinGame(\'{@id}\');return false;"><xsl:value-of select="@name"/></a>\
			<xsl:if test="@started=1">\
			[started]\
			</xsl:if>\
			(\
			<b> <xsl:value-of select="@owner"/></b>,\
			<xsl:for-each select="player">\
				<xsl:value-of select="@name"/>,\
			</xsl:for-each>\
			)<br/>\
		</xsl:for-each>\
		<br/><button type="button" onclick="SendCreateGame()">Create Game</button>\
	'
	SetDivFromCommandElem(document.getElementById('game_list_content'), elem, xsl) 
}

function OnCommandUpdateLobby(elem)
{		
	var xsl = '\
		<h2><xsl:value-of select="@game"/></h2><br/>\
		<b> <xsl:value-of select="@owner"/></b>,\
		<xsl:for-each select="player">\
			<xsl:value-of select="@name"/>,\
		</xsl:for-each>\
	'
	SetDivFromCommandElem(document.getElementById('lobby_content'), elem, xsl) 
}

function OnCommandUpdateLobbyControls(elem)
{	
	var show = IsTrue(elem.getAttribute('show'))
	var html = show ? '<br/><br/><button type="button" onclick="SendStartGame()">Start Game</button>' : ''
	document.getElementById('lobby_owner_controls').innerHTML = html
}

function OnCommandUpdateChooseTeam(elem)
{		
	var xsl = '\
		<h2><xsl:value-of select="@game"/></h2><br/>\
		<xsl:for-each select="team">\
			<xsl:value-of select="@name"/>:\
			<xsl:value-of select="@race"/>:\
			<xsl:value-of select="@colour"/>\
			<br/>\
		</xsl:for-each>\
	'
	SetDivFromCommandElem(document.getElementById('choose_team_content'), elem, xsl) 
}

function OnCommandUpdateTeams(elem)
{		
	data.team_count = 0
	data.team_pages = {}
	
	var fmt_tab = '<button type="button" onclick="ShowTeamPage(\'{0}\')">{1}</button>'
	var fmt_page = '\
					<div id="{0}">\
						<div id="{0}_summary"></div>\
						<div id="{0}_storage"></div>\
						<div id="{0}_population"></div>\
						<div id="{0}_technology"></div>\
						<div id="{0}_reputation"></div>\
						<div id="{0}_influence" onclick="if (data.action && data.action.OnClickInfluenceTrack) data.action.OnClickInfluenceTrack()"></div>\
						<div id="{0}_passed"></div>\
					</div>'
	
	var html_tabs = '', html_pages = ''

	var teams = GetChildElements(elem, 'team')
	for (var i = 0; i < teams.length; ++i)
	{
		var name = teams[i].getAttribute('name')
		var id = teams[i].getAttribute('id')
		
		html_tabs += fmt_tab.format(id, name)
		html_pages += fmt_page.format(GetTeamPageIDFromIndex(data.team_count))

		data.team_pages[id] = data.team_count++
	}

	html_tabs += '<button type="button" onclick="ShowSupplyPage()">Supply</button>'
	
	document.getElementById('game_tabs').innerHTML = html_tabs
	document.getElementById('game_pages').innerHTML = html_pages
	
	ShowTeamPage(data.playerID)

	var game_type = elem.getAttribute('game_type')
	ShowElementById('live_ui', game_type == "live")
	ShowElementById('review_ui', game_type == "review")
}

function OnCommandUpdateTeam(elem)
{		
	var xsl = '\
		<b>Team:</b> <xsl:value-of select="@name"/><br/>\
		<b>Race:</b> <xsl:value-of select="@race"/><br/>\
		<b>Colour:</b> <xsl:value-of select="@colour"/><br/>\
		<br/>\
	'
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'summary')), elem, xsl)
}

function OnCommandUpdatePassed(elem)
{
	var xsl = '\
		<b>Passed:</b> <xsl:value-of select="@has_passed"/><br/>\
	'
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'passed')), elem, xsl)
}

function OnCommandUpdateInfluenceTrack(elem)
{
	var xsl = '\
		<b>Influence discs:</b> <xsl:value-of select="@discs"/><br/>\
	'
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'influence')), elem, xsl)
}

function OnCommandUpdateStorageTrack(elem)
{
	var xsl = '\
		<b>Storage:</b>\
		Money: <xsl:value-of select="@Money"/>\
		Science: <xsl:value-of select="@Science"/>\
		Materials: <xsl:value-of select="@Materials"/>\
		<br/>'
		
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'storage')), elem, xsl)
}

function OnCommandUpdateTechnologyTrack(elem)
{
	var xsl = '\
		<b>Technology:</b><br/>\
		<xsl:for-each select="class">\
			<xsl:value-of select="@name"/>:\
			<xsl:for-each select="tech">\
				<xsl:value-of select="@name"/>, \
			</xsl:for-each>\
			<br/>\
		</xsl:for-each>'
		
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'technology')), elem, xsl)
}

function OnCommandUpdatePopulationTrack(elem)
{
	var xsl = '\
		<b>Population:</b>\
		Money: <xsl:value-of select="@Money"/>\
		Science: <xsl:value-of select="@Science"/>\
		Materials: <xsl:value-of select="@Materials"/>\
		<br/>'
		
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'population')), elem, xsl)
}

function OnCommandUpdateReputationTrack(elem)
{
	var xsl = '\
		<b>Reputation:</b>\
		Tiles: <xsl:value-of select="@tiles"/>/<xsl:value-of select="@slots"/>\
		send_values: <xsl:value-of select="@send_values"/>\
		<br/>'
		
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'reputation')), elem, xsl)
}

function OnCommandUpdateMap(elem)
{
	Map.Clear()
	
	var hexes = GetChildElements(elem, 'hex')
	for (var i = 0; i < hexes.length; ++i)
	{
		var id = hexes[i].getAttribute('id')
		var x = hexes[i].getAttribute('x')
		var y = hexes[i].getAttribute('y')
		var rotation = hexes[i].getAttribute('rotation')
		var team = hexes[i].getAttribute('colour')

		var squares = []
		var squares_elem = GetFirstChildElement(hexes[i], 'squares')
		if (squares_elem)
		{
			var square_elems = GetChildElements(squares_elem, 'square')
			for (var j = 0; j < square_elems.length; ++j)
			{
				var x2 = square_elems[j].getAttribute('x')
				var y2 = square_elems[j].getAttribute('y')
				squares.push(new Point(x2, y2))
			}
		}

		var ships = []
		var ships_elem = GetFirstChildElement(hexes[i], 'ships')
		if (ships_elem)
		{
			var ship_elems = GetChildElements(ships_elem, 'ship')
			for (var j = 0; j < ship_elems.length; ++j)
			{
				var colour = ship_elems[j].getAttribute('colour')
				var type = ship_elems[j].getAttribute('type')
				ships.push({ colour:colour, type:type })
			}
		}

		Map.AddHex(id, new Point(x, y), rotation, team, squares, ships)
	}
	Map.Draw()
}

function OnCommandUpdateReviewUI(elem)
{
	document.getElementById('retreat_review').disabled = !IsTrue(elem.getAttribute('can_retreat'))
	document.getElementById('advance_review').disabled = !IsTrue(elem.getAttribute('can_advance'))
}

function OnCommandUpdateTechnologies(elem)
{
	var div = document.getElementById('supply_technologies')
	var xsl = '\
		<xsl:for-each select="tech">\
			<xsl:value-of select="@count"/> x <xsl:value-of select="@type"/> (<xsl:value-of select="@max_cost"/>/<xsl:value-of select="@min_cost"/>)<br/>\
		</xsl:for-each>'

	SetDivFromCommandElem(div, elem, xsl) 
}

function OnCommandChooseTeam(elem, active)
{		
	var div = document.getElementById('choose_team')
	var xsl = ''
	if (active)
	{
		xsl = '\
			<div>\
			Race: <select id="select_race">\
			<xsl:for-each select="race">\
				<option value="{@name},{@colour}"><xsl:value-of select="@name"/> (<xsl:value-of select="@colour"/>)</option>\
			</xsl:for-each>\
			</select>\
			<button type="button" onclick="SendChooseTeam()">OK</button>\
			</div>\
		'
	}
	SetDivFromCommandElem(div, elem, xsl) 
}

function OnCommandChooseAction(elem)
{
	document.getElementById('choose_undo_btn').disabled = !IsTrue(elem.getAttribute('can_undo'))
	document.getElementById('choose_action_explore_btn').disabled = !IsTrue(elem.getAttribute('can_explore'))
	document.getElementById('choose_action_influence_btn').disabled = !IsTrue(elem.getAttribute('can_influence'))
	document.getElementById('choose_action_research_btn').disabled = !IsTrue(elem.getAttribute('can_research'))
	document.getElementById('choose_action_upgrade_btn').disabled = !IsTrue(elem.getAttribute('can_upgrade'))
	document.getElementById('choose_action_build_btn').disabled = !IsTrue(elem.getAttribute('can_build'))
	document.getElementById('choose_action_move_btn').disabled = !IsTrue(elem.getAttribute('can_move'))
	document.getElementById('choose_action_colonise_btn').disabled = !IsTrue(elem.getAttribute('can_colonise'))
	document.getElementById('choose_action_diplomacy_btn').disabled = !IsTrue(elem.getAttribute('can_diplomacy'))
	document.getElementById('choose_action_trade_btn').disabled = !IsTrue(elem.getAttribute('can_trade'))
	document.getElementById('choose_action_pass_btn').disabled = !IsTrue(elem.getAttribute('can_pass'))
	document.getElementById('choose_end_turn_btn').disabled = !IsTrue(elem.getAttribute('can_end_turn'))

	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	ShowActionElement('choose_action')
}

function OnCommandChooseFinished(elem)
{
	ShowElementById('choose_subaction', false)
	ShowElementById('choose_undo', false)
	ShowActionElement(null)
}

