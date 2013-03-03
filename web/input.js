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
	else if (type == 'action')
		OnCommandAction(elem)
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
}

function OnCommandUpdate(elem)
{
	var param = elem.getAttribute('param')

	if (param == "game_list")
		OnCommandUpdateGameList(elem)
	else if (param == "lobby")
		OnCommandUpdateLobby(elem)
	else if (param == "choose_team")
		OnCommandUpdateChooseTeam(elem)
	else if (param == "game")
		OnCommandUpdateGame(elem)
	else
        writeToScreen('OnCommandUpdate: unknown param: ' + param)
}

function OnCommandAction(elem)
{
	var param = elem.getAttribute('param')
	var active = elem.getAttribute('active') == '1'

	if (param == "choose_team")
		OnCommandActionChooseTeam(elem, active)
	else
        writeToScreen('OnCommandAction: unknown param: ' + param)
}

function OnCommandUpdateGameList(elem)
{
	var xsl = '\
		<xsl:for-each select="game">\
			<a href="Join Game" onclick="SendJoinGame(\'{@name}\');return false;"><xsl:value-of select="@name"/></a>\
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
	
	if (elem.getAttribute('owner') == playerName)
	{
		xsl = '<br/><br/><button type="button" onclick="SendStartGame(\'{@game}\')">Start Game</button>'
		SetDivFromCommandElem(document.getElementById('lobby_owner_controls'), elem, xsl) 
	}
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
	SetDivFromCommandElem(document.getElementById('choose_content'), elem, xsl) 
}

function OnCommandUpdateGame(elem)
{		
	var xsl = '\
		<h2><xsl:value-of select="@game"/></h2><br/>\
		<b> <xsl:value-of select="@owner"/></b>,\
		<xsl:for-each select="player">\
			<xsl:value-of select="@name"/>,\
		</xsl:for-each>\
	'
	SetDivFromCommandElem(document.getElementById('game_content'), elem, xsl) 
}

function OnCommandActionChooseTeam(elem, active)
{		
	var div = document.getElementById('choose_action')
	var xsl = ''
	if (active)
	{
		xsl = '\
			<div>\
			Race: <select id="select_race">\
			<xsl:for-each select="races/race">\
				<option value="{@name}"><xsl:value-of select="@name"/></option>\
			</xsl:for-each>\
			</select>\
			Colour: <select id="select_colour">\
			<xsl:for-each select="colours/colour">\
				<option value="{@name}"><xsl:value-of select="@name"/></option>\
			</xsl:for-each>\
			</select>\
			<button type="button" onclick="SendChooseTeam()">OK</button>\
			</div>\
		'
	}
	SetDivFromCommandElem(div, elem, xsl) 
}
