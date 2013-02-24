function SetDivFromXML(div, elem, xsl)
{		
	var xsl2 = '<?xml version="1.0"?><xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">'
	xsl2 += xsl + '</xsl:stylesheet>'
	
	xslNode = parser.parseFromString(xsl2, "text/xml");
  
	xsltProcessor = new XSLTProcessor()
	xsltProcessor.importStylesheet(xslNode)
	var htmlNode = xsltProcessor.transformToFragment(elem, document)

	div.innerHTML = ''
	div.appendChild(htmlNode)
}

function SetDivFromCommandElem(div, elem, xsl)
{		
	SetDivFromXML(div, elem, '<xsl:template match="/command">' + xsl + '</xsl:template>')
}

function OnCommand(elem)
{
	var type = elem.getAttribute('type')

	if (type == 'show')
		OnCommandShow(elem)
	else if (type == 'update')
		OnCommandUpdate(elem)
	else
		writeToScreen('OnCommand: unknown command: ' + type)
}

function OnCommandShow(elem)
{
	var panel = elem.getAttribute('panel')

	var panels =['game_list_panel', 'lobby_panel', 'game_panel']
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
	else if (param == "game")
		OnCommandUpdateGame(elem)
	else
        writeToScreen('OnCommandUpdate: unknown param: ' + param)
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
