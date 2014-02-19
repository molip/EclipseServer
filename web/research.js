var Research = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Research.Stage = function()
{
	ClearSelect(document.getElementById('select_tech'))
}

Research.Stage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_research')

	var idx = document.getElementById('select_tech').selectedIndex
	node.setAttribute("tech_idx", idx)
	
	ExitAction()

	SendXMLDoc(doc)
}

Research.Stage.prototype.CleanUp = function()
{
}

Research.ArtifactStage = function(count)
{
	this.count = count
	this.array = [0, 0, 0]
	
	this.UpdateCtrls()
}

Research.ArtifactStage.prototype.UpdateCtrls = function()
{
	function Update(idx, array, count)
	{	
		var str = 'choose_research_artifact_{0}{1}'
		document.getElementById(str.format('text', idx)).value = array[idx]
		document.getElementById(str.format('minus', idx)).disabled = array[idx] == 0
		document.getElementById(str.format('plus', idx)).disabled = count == 0
	}

	Update(0, this.array, this.count)
	Update(1, this.array, this.count)
	Update(2, this.array, this.count)

	document.getElementById('choose_research_artifactkey_count').innerHTML = this.count
	document.getElementById('choose_research_artifact_next').disabled = this.count != 0
}

Research.ArtifactStage.prototype.Send = function()
{
	var doc = CreateXMLDoc()
	var node = CreateCommandNode(doc, 'cmd_research_artifact')

	node.setAttribute("Money", this.array[0])
	node.setAttribute("Science", this.array[1])
	node.setAttribute("Materials", this.array[2])
	
	ExitAction()

	SendXMLDoc(doc)
}

Research.ArtifactStage.prototype.CleanUp = function()
{
}

Research.ArtifactStage.prototype.OnButton = function(button, resource, delta)
{
	this.array[resource] += delta
	this.count -= delta
	this.UpdateCtrls()
}

///////////////////////////////////////////////////////////////////////////////
// Input

Research.OnCommand = function(elem)
{
	ShowActionElement('choose_research')

	ShowElementById('choose_research_reject_btn', elem.can_skip, true)

	data.action = new Research.Stage()

	var select = document.getElementById('select_tech')
	
	for (var i = 0, tech; tech = elem.techs[i]; ++i)
		select.add(new Option('{0} ({1})'.format(tech.type, tech.cost), ''))
}

Research.OnCommandArtifact = function(elem)
{
	ShowActionElement('choose_research_artifact')

	data.action = new Research.ArtifactStage(elem.count)
}
