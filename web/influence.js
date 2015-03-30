var Influence = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Influence.Stage = function(positions, can_select_track, can_abort, max_flips, is_dst)
{
	this.positions = positions
	this.flagNoSubAction = !can_abort
	this.can_select_track = can_select_track
	this.max_flips = max_flips
	this.btn_id = is_dst ? 'choose_influence_dst_btn' : 'choose_influence_src_btn', 
	this.is_dst = is_dst
	this.selected = null
	this.track_selected = false
	this.pos_idx = can_select_track ? -1 : 0

	document.getElementById(this.btn_id).disabled = true
	document.getElementById('choose_influence_finish_btn').disabled = !can_abort
	
	var flipBtn = document.getElementById('choose_influence_flip_btn')
	flipBtn.disabled = max_flips <= 0
	flipBtn.textContent = 'Flip {0} colony ship{1}'.format(max_flips, max_flips == 1 ? '' : 's')
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
	var json = CreateCommandJSON(this.is_dst ? 'cmd_influence_dst' : 'cmd_influence_src')
	json.pos_idx = this.pos_idx

	ExitAction()

	SendJSON(json)
}

Influence.Stage.prototype.SendFlip = function()
{
	var json = CreateCommandJSON('cmd_influence_flip')
	ExitAction()
	SendJSON(json)
}

Influence.Stage.prototype.UpdateTrackSelection = function()
{
	if (IsCurrentTeam(data.playerID))
		Team.UpdateInfluence()
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

	data.action = new Influence.Stage(elem.positions, elem.can_select_track, elem.can_abort, elem.max_flips, false)
	data.action.UpdateTrackSelection()
	Map.DrawActionLayer()
}

Influence.OnCommandChooseDst = function(elem)
{
	ShowActionElement('choose_influence_dst')
	Map.selecting = true

	data.action = new Influence.Stage(elem.positions, elem.can_select_track, 0, true)
	data.action.UpdateTrackSelection()
	Map.DrawActionLayer()
}
