var Move = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Move.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false
}

Move.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_move')

	ExitAction()

	SendXMLDoc(doc)
}

Move.Stage.prototype.CleanUp = function()
{
}

///////////////////////////////////////////////////////////////////////////////
// Input

Move.OnCommand = function(elem)
{
	ShowActionElement('choose_move')
	data.action = new Move.Stage()
}
