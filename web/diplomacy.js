var Diplomacy = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Diplomacy.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false
}

Diplomacy.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_diplomacy')

	ExitAction()

	SendXMLDoc(doc)
}

Diplomacy.Stage.prototype.CleanUp = function()
{
}

///////////////////////////////////////////////////////////////////////////////
// Input

Diplomacy.OnCommand = function(elem)
{
	ShowActionElement('choose_diplomacy')
	data.action = new Diplomacy.Stage()
}
