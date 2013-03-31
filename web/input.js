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
	else if (param == "lobby_controls")
		OnCommandUpdateLobbyControls(elem)
	else if (param == "choose_team")
		OnCommandUpdateChooseTeam(elem)
	else if (param == "teams")
		OnCommandUpdateTeams(elem)
	else if (param == "team")
		OnCommandUpdateTeam(elem)
	else if (param == "map")
		OnCommandUpdateMap(elem)
	else
        writeToScreen('OnCommandUpdate: unknown param: ' + param)
}

function OnCommandAction(elem)
{
	var param = elem.getAttribute('param')
	var active = IsTrue(elem.getAttribute('active'))

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
	SetDivFromCommandElem(document.getElementById('choose_content'), elem, xsl) 
}

function OnCommandUpdateTeams(elem)
{		
	data.team_count = 0
	data.team_pages = {}
	
	var fmt_tab = '<button type="button" onclick="ShowTeamPage(\'{0}\')">{0}</button>'
	var fmt_page = '<div id="{0}"></div>'
	
	var html_tabs = '', html_pages = ''

	var i = 0
	for (var team = elem.firstChild; team; team = team.nextSibling)
	{
		if (team.nodeName == "team")
		{
			var name = team.getAttribute('name')
			
			html_tabs += fmt_tab.format(name)
			html_pages += fmt_page.format(GetTeamPageIDFromIndex(data.team_count))

			data.team_pages[name] = data.team_count++
		}
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
	SetDivFromCommandElem(document.getElementById(GetTeamPageIDFromName(elem.getAttribute('name'))), elem, xsl)
}

function OnCommandUpdateMap(elem)
{
	//hexes = []

	var xsl = '\
		<xsl:for-each select="hex">\
			<img id="hex_{@id}" src="/images/hexes/{@id}.png"/>\
		</xsl:for-each>\
	'
	SetDivFromCommandElem(document.getElementById('images'), elem, xsl) 
	
	var size_x = data.hex_width, size_y = data.hex_height
	
	var canvas = document.getElementById('map_canvas')
	var ctx = canvas.getContext("2d");
	ctx.setTransform(1, 0, 0, 1, 0, 0)
	ctx.clearRect(0, 0, 600, 600);

	ctx.translate(300, 300)
	ctx.scale(0.3, 0.3)
	
	var img = new Image()
	for (var hex = elem.firstChild; hex; hex = hex.nextSibling)
	{
		if (hex.nodeName == "hex")
		{
			var id = hex.getAttribute('id')
			var x = Number(hex.getAttribute('x'))
			var y = Number(hex.getAttribute('y'))

			var p = GetHexCentre(x, y)
			
			//var img = document.getElementById("hex_" + id);
			img.src = "/images/hexes/" + id + ".png"
			ctx.translate(-size_x / 2, -size_y / 2)
			ctx.drawImage(img, p.x, p.y);
			ctx.translate(size_x / 2, size_y / 2)
			
			//writeToScreen("hex {0}: {1}, {2}\n".format(id, p.x, p.y))
		}
	}
	var canvas2 = document.getElementById('map_canvas2')
	canvas2.addEventListener("mousemove", OnMouse)
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
