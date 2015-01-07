var Discovery = {}

Discovery.ChooseDiscoveryStage = function(elem)
{
	this.flagNoSubAction = true;
	this.flagCantUndo = true
	
	document.getElementById('choose_discovery_item').innerText = elem.discovery
	document.getElementById('choose_discovery_use').disabled = !elem.can_use
	document.getElementById('choose_discovery_keep').disabled = !elem.can_keep
}

Discovery.ChooseDiscoveryStage.prototype.SendDiscovery = function(action)
{
	var json = CreateCommandJSON('cmd_discovery')
	json.action = action

	ExitAction()
	
	SendJSON(json)
}

///////////////////////////////////////////////////////////////////////////////
// Input

Discovery.OnCommandChooseDiscovery = function(elem)
{
	data.action = new Discovery.ChooseDiscoveryStage(elem)

	ShowActionElement('choose_discovery')
	
	Map.DrawActionLayer()
	Map.DrawSelectLayer()
}

