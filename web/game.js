var output
var ws

if(!String.prototype.format) {
  String.prototype.format = function() {
    var args = arguments;
    return this.replace(/{(\d+)}/g, function (match, number) {
          return typeof args[number]!= 'undefined'
        ? args[number]
        : match
		;
});
};
}

function SendMessage()
{
	var textbox = document.getElementById('textbox'); 
	ws.send(textbox.value);
	textbox.value = '';
}

function writeToScreen(message)
{
	var pre = document.createElement("p");
	pre.style.wordWrap = "break-word";
	pre.innerHTML = message;
	output.appendChild(pre);
}
function OnMessage(msg)
{
	var tokens = msg.data.split(':')
	var cmd = tokens.shift()

	writeToScreen('Received message: ' + msg.data + '\n')

	if (cmd == 'SHOW')
		OnCmdShow(tokens)
	else if (cmd == 'UPDATE')
		OnCmdUpdate(tokens)
	else
		writeToScreen('Error: unknown command: ' + cmd + '\n')
}
  
function OnClose()
{
	writeToScreen('Closed\n')
}

function load()
{
	output = document.getElementById("output");

	if ("WebSocket" in window)
	{
		ws = new WebSocket("ws://localhost:8998/echo");
		ws.onopen = function() { ws.send('REGISTER:' + playerName); }
		ws.onmessage = OnMessage;
		ws.onclose = OnClose;
	}
	else
	{
		alert("WebSocket not supported by your Browser!");
	}
}

//-----------------------------------------------------------------------------

function OnCmdShow(params)
{
	var panels =['PANEL_GAMELIST', 'PANEL_GAME']
    var found = false
    for (var i = 0; i < panels.length; ++i)
	{
    	//alert(i, panels[i])
		var div = document.getElementById(panels[i])
		if (params[0] == panels[i])
		{
			writeToScreen('OnCmdShow: showing ' + panels[i] + '\n')
		    div.style.display = "block"
		    found = true
		}
		else
		{
			writeToScreen('OnCmdShow: hiding ' + panels[i] + '\n')
		    div.style.display = "none" 
		} 
	}
    if(!found)
        writeToScreen('Error: OnCmdShow: unknown element: ' + params[0] + '\n')
}

function OnCmdUpdate(params)
{
	var type = params.shift()
	
	if (type == "GAMELIST")
	{
		var html = ''
		var f = '{0} <button type="button" onclick="ws.send(\'JOIN_GAME:{0}\')">Join</button><br/>'
		for (var i = 0; i < params.length; ++i)
			html += f.format(params[i])

		var elem = document.getElementById('GameListContent')
		elem.innerHTML = html
	}
}
