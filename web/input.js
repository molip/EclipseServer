
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
	if (param == "lobby")
		OnCommandUpdateLobby(elem)
	if (param == "game")
		OnCommandUpdateGame(elem)
	else
        writeToScreen('OnCommandUpdate: unknown param: ' + param)
}

function OnCommandUpdateGameList(elem)
{		
	var html = ''

	var f = '<a href="Join Game" onclick="SendJoinGame(\'{0}\');return false;">{0}</a>'

	var game = elem.firstChild
	while (game)
	{
		if (game.nodeName == "game")
		{
			var owner = game.getAttribute('owner')
			html += f.format(game.getAttribute('name'))
			html += ' Players: <b>' + owner + '</b>'

			var player = game.firstChild
			while (player)
			{
				if (player.nodeName == "player")
				{
					var name = player.getAttribute('name')
					if (name != owner)
						html += ', ' + name
				}

				player = player.nextSibling
			}
			html += '<br/>'
		}
		game = game.nextSibling
	}
	
	html += '<br/><button type="button" onclick="SendCreateGame()">Create Game</button>'
	
	var elem = document.getElementById('game_list_content')
	elem.innerHTML = html
}

function OnCommandUpdateLobby(elem)
{		
	var html = ''
	var owner = elem.getAttribute('owner')
	html += '<b>' + owner + '</b>'

	var player = elem.firstChild
	while (player)
	{
		if (player.nodeName == "player")
		{
			var name = player.getAttribute('name')
			if (name != owner)
				html += ', ' + name
		}

		player = player.nextSibling
	}

	document.getElementById('lobby_game_name').innerHTML = elem.getAttribute('game')
	document.getElementById('lobby_content').innerHTML = html
}

function OnCommandUpdateGame(elem)
{		
}
