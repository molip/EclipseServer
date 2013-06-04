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

function CreateCommandDoc(type)
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, type)
	return doc
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
	SendXMLDoc(CreateCommandDoc("create_game"))
}

function SendExitGame()
{
	ExitAction()
	
	SendXMLDoc(CreateCommandDoc("exit_game"))
}

function SendStartReview()
{
	ExitAction()

	SendXMLDoc(CreateCommandDoc("start_review"))
}

function SendExitReview()
{
	SendXMLDoc(CreateCommandDoc("exit_review"))
}

function SendAdvanceReview()
{
	SendXMLDoc(CreateCommandDoc("advance_review"))
}

function SendRetreatReview()
{
	SendXMLDoc(CreateCommandDoc("retreat_review"))
}

function SendStartGame()
{
	SendXMLDoc(CreateCommandDoc("start_game"))
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
	SendXMLDoc(CreateCommandDoc("commit"))
}

function SendChooseUndo()
{
	ExitAction()

	SendXMLDoc(CreateCommandDoc("undo"))
}

function SendRegister()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, "register")
	
	node.setAttribute("player", data.playerID)
	//AddTextElem(doc, node, "player", data.playerID)

	SendXMLDoc(doc)
}