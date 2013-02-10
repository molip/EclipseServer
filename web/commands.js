
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

	var panels =['game_list_panel', 'game_panel']
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
	else
        writeToScreen('OnCommandUpdate: unknown param: ' + param)
}

function OnCommandUpdateGameList(elem)
{		
	var html = ''

	//var f = '{0} <button type="button" onclick="ws.send(\'JOIN_GAME:{0}\')">Join</button><br/>'
	var f = '<a href="Join Game" onclick="ws.send(\'JOIN_GAME:{0}\');return false;">{0}</a><br/>'

	var game = elem.firstChild
	while (game)
	{
		if (game.nodeName == "game")
			html += f.format(game.getAttribute('name'))
		game = game.nextSibling
	}
	
	html += '<br/><button type="button" onclick="ws.send(\'CREATE_GAME\')">Create Game</button>'
	
	var elem = document.getElementById('game_list_content')
	elem.innerHTML = html
}

