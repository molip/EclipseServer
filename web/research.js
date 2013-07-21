var Research = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Research.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false

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
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false

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

	var can_skip = IsTrue(elem.getAttribute('can_skip'))
	ShowElementById('choose_research_reject_btn', can_skip, true)

	data.action = new Research.Stage()

	var select = document.getElementById('select_tech')
	
	var elems = GetChildElements(elem, 'tech')
	//alert(elems.length)
	for (var i = 0; i < elems.length; ++i)
	{
		var type = elems[i].getAttribute('type')
		var cost = elems[i].getAttribute('cost')
		select.add(new Option('{0} ({1})'.format(type, cost), ''))
	}
}

Research.OnCommandArtifact = function(elem)
{
	ShowActionElement('choose_research_artifact')

	var artifacts = elem.getAttribute('count')
	data.action = new Research.ArtifactStage(artifacts)
}
