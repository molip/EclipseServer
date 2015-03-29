var AutoInfluence = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

AutoInfluence.Stage = function(positions)
{
	this.flagNoSubAction = true;
	this.flagCantUndo = true

	this.positions = positions
	this.isSelected = []
	for (var i = 0; i < positions.length; ++i)
		this.isSelected.push(false)
}

AutoInfluence.Stage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (pt.equals(this.positions[i]))
		{
			this.isSelected[i] = !this.isSelected[i]
			Map.DrawActionLayer()
			Team.UpdateInfluence()
			return true
		}
	return false
}

AutoInfluence.Stage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.positions.length; ++i)
	{
		Map.DrawCentred(ctx, Map.img_explore, this.positions[i])
		if (this.isSelected[i])
			Map.DrawInfluenceDisc(ctx, this.positions[i], GetCurrentTeam().colour)
	}
}

AutoInfluence.Stage.prototype.ModifyInfluence = function(discs)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (this.isSelected[i])
			--discs
	return discs
}

AutoInfluence.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON('cmd_auto_influence')
	json.selected = this.isSelected

	ExitAction()

	SendJSON(json)
}

AutoInfluence.Stage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_select)
	Map.ClearCanvas(Map.layer_action)
	Team.UpdateInfluence()
}

///////////////////////////////////////////////////////////////////////////////
// Input

AutoInfluence.OnCommand = function(elem)
{
	ShowActionElement('choose_auto_influence')
	Map.selecting = true

	data.action = new AutoInfluence.Stage(elem.positions)
	Map.DrawActionLayer()
}
