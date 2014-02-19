var Upgrade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Upgrade.Stage = function()
{
}

Upgrade.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_upgrade')

	ExitAction()

	SendJSON(json)
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
