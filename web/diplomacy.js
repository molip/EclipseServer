var Diplomacy = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Diplomacy.Stage = function()
{
	this.flagNoSubAction = true;
}

Diplomacy.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_diplomacy')

	ExitAction()

	SendJSON(json)
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
