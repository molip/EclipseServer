var Diplomacy = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Diplomacy.Stage = function()
{
	this.flagNoSubAction = true;
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
