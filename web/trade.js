var Trade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Trade.Stage = function()
{
	this.flagNoSubAction = true;
}

Trade.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_trade')
	
	ExitAction()

	SendJSON(json)
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
