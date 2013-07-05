var Trade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Trade.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false
}

Trade.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_trade')

	ExitAction()

	SendXMLDoc(doc)
}

Trade.Stage.prototype.CleanUp = function()
{
}

///////////////////////////////////////////////////////////////////////////////
// Input

Trade.OnCommand = function(elem)
{
	ShowActionElement('choose_trade')
	data.action = new Trade.Stage()
}
