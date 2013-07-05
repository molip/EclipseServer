var Build = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Build.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false
}

Build.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_build')

	ExitAction()

	SendXMLDoc(doc)
}

Build.Stage.prototype.CleanUp = function()
{
}

///////////////////////////////////////////////////////////////////////////////
// Input

Build.OnCommand = function(elem)
{
	ShowActionElement('choose_build')
	data.action = new Build.Stage()
}
