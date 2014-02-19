var Influence = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Influence.Stage = function(positions, can_select_track, btn_id, command)
{
	this.positions = positions
	this.can_select_track = can_select_track
	this.btn_id = btn_id
	this.command = command
	this.selected = null
	this.track_selected = false
	this.pos_idx = can_select_track ? -1 : 0

	document.getElementById(btn_id).disabled = true

	this.UpdateTrackSelection()
}

Influence.Stage.prototype.OnClickInfluenceTrack = function()
{
	if (this.can_select_track)
	{
		this.selected = null
		this.pos_idx = -1
		this.track_selected = true
		this.UpdateTrackSelection()
		Map.DrawSelectLayer()
		document.getElementById(this.btn_id).disabled = false
	}
}

Influence.Stage.prototype.OnHexMouseDown = function(pt)
{
	for (var i = 0; i < this.positions.length; ++i)
		if (pt.equals(this.positions[i]))
		{
			this.selected = pt.Clone()
			this.pos_idx = i
			this.track_selected = false
			this.UpdateTrackSelection()
			document.getElementById(this.btn_id).disabled = false
			return true
		}
	return false
}

Influence.Stage.prototype.OnDraw = function(ctx)
{
	for (var i = 0; i < this.positions.length; ++i)
		Map.DrawCentred(ctx, Map.img_explore, this.positions[i])
}

Influence.Stage.prototype.Send = function()
{
	var json = CreateCommandJSON(this.command)
	json.pos_idx = this.pos_idx

	ExitAction()

	SendJSON(json)
}

Influence.Stage.prototype.UpdateTrackSelection = function()
{
	var style = ""
	
	if (this.can_select_track)
		style = this.track_selected ? "2px solid red" : "2px solid green"
		
	document.getElementById(GetTeamDivIDFromName(data.playerID, 'influence')).style.border = style
}

Influence.Stage.prototype.CleanUp = function()
{
	Map.ClearCanvas(Map.layer_select)
	Map.ClearCanvas(Map.layer_action)
	this.track_selected = this.can_select_track = false
	this.UpdateTrackSelection()
}

///////////////////////////////////////////////////////////////////////////////
// Input

Influence.OnCommandChooseSrc = function(elem)
{
	ShowActionElement('choose_influence_src')
	Map.selecting = true

	data.action = new Influence.Stage(elem.positions, elem.can_select_track, 'choose_influence_src_btn', 'cmd_influence_src')
	Map.DrawActionLayer()
}

Influence.OnCommandChooseDst = function(elem)
{
	ShowActionElement('choose_influence_dst')
	Map.selecting = true

	data.action = new Influence.Stage(elem.positions, elem.can_select_track, 'choose_influence_dst_btn', 'cmd_influence_dst')
	Map.DrawActionLayer()
}
