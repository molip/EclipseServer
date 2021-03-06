function CreateCommandJSON(type)
{
	return { "type": type }
}

function SendJSON(json, noBlanket)
{
	var str = JSON.stringify(json)
	ws.send(str)

	if (!noBlanket)
		ShowBlanket(true)
}

function SendEnterGame(id)
{
	var json = CreateCommandJSON('enter_game')
	json.game = id
	SendJSON(json)
}

function SendCreateGame()
{
	SendJSON(CreateCommandJSON('create_game'))
}

function SendExitGame()
{
	ExitAction()
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

function SendJoinGame()
{
	SendJSON(CreateCommandJSON('join_game'))
}

function SendUnjoinGame()
{
	SendJSON(CreateCommandJSON('unjoin_game'))
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
	ExitAction(true)

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

function SendChat()
{
	var json = CreateCommandJSON('chat')

	var input = document.getElementById('chat')
	json.message = input.value
	input.value = ''

	SendJSON(json, true)
}