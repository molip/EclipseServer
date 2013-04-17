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
	else if (param == "map")
		OnCommandUpdateMap(elem)
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
	// else if (param == "commit")
		// OnCommandChooseCommit(elem)
	else if (param == "explore_pos")
		Explore.OnCommandChoosePos(elem)
	else if (param == "explore_hex")
		Explore.OnCommandChooseHex(elem)
	else if (param == "explore_discovery")
		Explore.OnCommandChooseDiscovery(elem)
	else if (param == "finished")
		OnCommandChooseFinished(elem)
	else
		writeToScreen('OnCommandChoose: unknown param: ' + param)
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
						<div id="{0}_influence"></div>\
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

	document.getElementById('game_tabs').innerHTML = html_tabs
	document.getElementById('game_pages').innerHTML = html_pages
	
	ShowTeamPage(data.playerID)
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

function OnCommandUpdateInfluenceTrack(elem)
{
	var xsl = '\
		<b>Influence discs:</b> <xsl:value-of select="@discs"/><br/>\
	'
	SetDivFromCommandElem(document.getElementById(GetTeamDivIDFromName(elem.getAttribute('id'), 'influence')), elem, xsl)
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

		Map.AddHex(id, new Point(x, y), rotation, team)
	}
	Map.Draw()
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
	// In case we got here via undo.
	ExitAction()
	
	ShowElementById('choose_undo', false)
	ShowActionElement('choose_action')
}

// function OnCommandChooseCommit(elem)
// {
	// ShowActionElement('choose_commit')
// }

function OnCommandChooseFinished(elem)
{
	ShowElementById('choose_undo', false)
	ShowActionElement(null)
}

