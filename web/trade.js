var Trade = {}

///////////////////////////////////////////////////////////////////////////////
// Stages

Trade.Stage = function(available, rate)
{
	this.flagNoSubAction = true;

	this.available = available // Money, Science, Materials
	this.rate = rate
	this.count = 0
	
	var select_from = document.getElementById('select_trade_from')
	var select_to = document.getElementById('select_trade_to')
	
	ClearSelect(select_from)
	ClearSelect(select_to)

	for (var type in available)
		select_from.add(new Option(type, type))

	this.UpdateSelectCtrls()
	this.UpdateCountCtrls()
}

Trade.Stage.prototype.UpdateCountCtrls = function()
{
	var select_from = document.getElementById('select_trade_from')
	
	var from = select_from.options[select_from.selectedIndex].value
	var max = Math.floor(this.available[from] / this.rate)

	if (this.count > max)
		this.count = max

	document.getElementById('choose_trade_text').value = this.count * this.rate
	document.getElementById('choose_trade_minus').disabled = this.count == 0
	document.getElementById('choose_trade_plus').disabled = this.count == max

	document.getElementById('trade_result_content').innerHTML = this.count
	document.getElementById('choose_trade_finish').disabled = this.count == 0
}

Trade.Stage.prototype.UpdateSelectCtrls = function()
{
	var select_from = document.getElementById('select_trade_from')
	var from = select_from.options[select_from.selectedIndex].value

	var select_to = document.getElementById('select_trade_to')
	
	if (select_to.options.length)
		var to = select_to.options[select_to.selectedIndex].value

	ClearSelect(select_to)

	var types = ['Money', 'Science', 'Materials']
	for (var i = 0, type; type = types[i]; ++i)
		if (type != from)
		{
			select_to.add(new Option(type, type))
			if (type == to)
				select_to.options[select_to.options.length - 1].selected = true
		}
}

Trade.Stage.prototype.OnSelectFrom = function()
{
	this.UpdateSelectCtrls()
	this.UpdateCountCtrls()
}

Trade.Stage.prototype.OnButton = function(button, delta)
{
	this.count += delta
	this.UpdateCountCtrls()
}

Trade.Stage.prototype.Send = function()
{
	var select_from = document.getElementById('select_trade_from')
	var from = select_from.options[select_from.selectedIndex].value

	var select_to = document.getElementById('select_trade_to')
	var to = select_to.options[select_to.selectedIndex].value

	var json = CreateCommandJSON('cmd_trade')
	json.from = from
	json.to = to
	json.count = this.count
	
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
	data.action = new Trade.Stage(elem.available, elem.rate)
}
