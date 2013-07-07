var Research = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Research.Stage = function()
{
	ShowElementById('choose_subaction', true)

	ShowElementById('choose_undo', true)
	document.getElementById('choose_undo_btn').disabled = false


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
	var select = document.getElementById('select_tech')
	while (select.length)
		select.remove(0)
}

///////////////////////////////////////////////////////////////////////////////
// Input

Research.OnCommand = function(elem)
{
	ShowActionElement('choose_research')
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
