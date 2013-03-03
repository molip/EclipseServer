function CreateXMLDoc()
{
	return document.implementation.createDocument(null, null, null)
}

function CreateCommandNode(doc, type)
{
	var root = doc.createElement("command")
	doc.appendChild(root)
	root.setAttribute("type", type)
	return root
}

function AddTextElem(doc, parent, name, text)
{
	var elem = doc.createElement(name)
	var textNode = doc.createTextNode(text)
	elem.appendChild(textNode)
	parent.appendChild(elem)
	return elem
}

function SendXMLDoc(doc)
{
	var str = new XMLSerializer().serializeToString(doc)
	writeToScreen("Sending: " + str + "\n")
	ws.send(str)
}

function SendJoinGame(game)
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "join_game")
	node.setAttribute("game", game)
	SendXMLDoc(doc)
}
function SendCreateGame()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "create_game")
	SendXMLDoc(doc)
}

function SendStartGame(game)
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "start_game")
	node.setAttribute("game", game)
	SendXMLDoc(doc)
}

function SendChooseTeam()
{
	var select_race = document.getElementById('select_race')
	var select_colour = document.getElementById('select_colour')
	
	var race = select_race.options[select_race.selectedIndex].value
	var colour = select_colour.options[select_colour.selectedIndex].value
	
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "choose_team")
	node.setAttribute("race", race)
	node.setAttribute("colour", colour)
	SendXMLDoc(doc)
}

function SendRegister()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "register")
	
	node.setAttribute("player", playerName)
	//AddTextElem(doc, node, "player", playerName)

	SendXMLDoc(doc)
}