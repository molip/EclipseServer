var Research = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Research.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false
}

Research.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_research')

	ExitAction()

	SendXMLDoc(doc)
}

Research.Stage.prototype.CleanUp = function()
{
}

///////////////////////////////////////////////////////////////////////////////
// Input

Research.OnCommand = function(elem)
{
	ShowActionElement('choose_research')
	data.action = new Research.Stage()
}
