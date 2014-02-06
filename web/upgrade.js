var Upgrade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Upgrade.Stage = function()
{
}

Upgrade.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_upgrade')

	ExitAction()

	SendXMLDoc(doc)
}

Upgrade.Stage.prototype.CleanUp = function()
{
}

///////////////////////////////////////////////////////////////////////////////
// Input

Upgrade.OnCommand = function(elem)
{
	ShowActionElement('choose_upgrade')
	data.action = new Upgrade.Stage()
}
