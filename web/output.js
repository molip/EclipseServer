function CreateCommandJSON(type)
{
	return { "type": type }
}

function SendJSON(json, noBlanket)
{
	var str = JSON.stringify(json)
	writeToScreen("Sending: " + str + "\n")
	ws.send(str)

	if (!noBlanket)
		ShowBlanket(true)
}

function SendJoinGame(id)
{
	var json = CreateCommandJSON('join_game')
	json.game = id
	SendJSON(json)
}

function SendCreateGame()
{
	SendJSON(CreateCommandJSON('create_game'))
}

function SendExitGame()
{
	SendJSON(CreateCommandJSON('exit_game'))
}

function SendStartReview()
{
	ExitAction()
	SendJSON(CreateCommandJSON('start_review'))
}

function SendExitReview()
{
	SendJSON(CreateCommandJSON('exit_review'))
}

function SendAdvanceReview()
{
	SendJSON(CreateCommandJSON('advance_review'))
}

function SendRetreatReview()
{
	SendJSON(CreateCommandJSON('retreat_review'))
}

function SendStartGame()
{
	SendJSON(CreateCommandJSON('start_game'))
}

function SendChooseTeam()
{
	var select_race = document.getElementById('select_race')
	var select_colour = document.getElementById('select_colour')
	
	var race_colour = select_race.options[select_race.selectedIndex].value.split(',')
	
	var json = CreateCommandJSON('choose_team')
	json.race = race_colour[0]
	json.colour = race_colour[1]
	SendJSON(json)
}

function SendChooseAction(action)
{
	var json = CreateCommandJSON('start_action')
	json.action = action
	SendJSON(json)
}

function SendChooseCommit()
{
	SendJSON(CreateCommandJSON('commit'))
}

function SendChooseUndo()
{
	ExitAction()

	SendJSON(CreateCommandJSON('undo'))
}

function SendFinishUpkeep()
{
	ExitAction()

	SendJSON(CreateCommandJSON('finish_upkeep'))
}

function SendRegister()
{
	var json = CreateCommandJSON('register')
	
	json.player = data.playerID

	SendJSON(json, true)
}

function SendAbort()
{
	ExitAction()

	SendJSON(CreateCommandJSON('cmd_abort'))
}