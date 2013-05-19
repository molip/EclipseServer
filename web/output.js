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

function SendJoinGame(id)
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "join_game")
	node.setAttribute("game", id)
	SendXMLDoc(doc)
}

function SendCreateGame()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "create_game")
	SendXMLDoc(doc)
}

function SendExitGame()
{
	ExitAction()
	
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "exit_game")
	SendXMLDoc(doc)
}

function SendStartGame()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "start_game")
	SendXMLDoc(doc)
}

function SendChooseTeam()
{
	var select_race = document.getElementById('select_race')
	var select_colour = document.getElementById('select_colour')
	
	var race_colour = select_race.options[select_race.selectedIndex].value.split(',')
	
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "choose_team")
	node.setAttribute("race", race_colour[0])
	node.setAttribute("colour", race_colour[1])
	SendXMLDoc(doc)
}

function SendChooseAction(action)
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "start_action")
	node.setAttribute("action", action)
	SendXMLDoc(doc)
}

function SendChooseCommit()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "commit")
	SendXMLDoc(doc)
}

function SendChooseUndo()
{
	ExitAction()

	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "undo")
	SendXMLDoc(doc)
}

function SendRegister()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "register")
	
	node.setAttribute("player", data.playerID)
	//AddTextElem(doc, node, "player", data.playerID)

	SendXMLDoc(doc)
}