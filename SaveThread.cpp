#include "stdafx.h"
#include "SaveThread.h"
#include "LiveGame.h"

SaveThread* SaveThread::s_instance;

void SaveThread::Push(const LiveGame& game)
{
	if (std::find(_queue.begin(), _queue.end(), &game) != _queue.end())
		return;
	
	{
		std::lock_guard<std::mutex> lock(_queueMutex);
		_queue.push_back(&game);
	}
	_cv.notify_one();
}

const LiveGame* SaveThread::Pop()
{
	const LiveGame* game = nullptr;

	std::lock_guard<std::mutex> lock(_queueMutex);
	if (!_queue.empty())
	{
		game = _queue.front();
		_queue.pop_front();
	}

	return game;
}

void SaveThread::Go()
{
	while (!_abort)
	{
		std::unique_lock<std::mutex> lock(_cvMutex);
		_cv.wait(lock);

		while (const LiveGame* game = Pop())
		{
			std::lock_guard<std::mutex> gameLock(game->GetMutex());
			game->Save();
			std::cout << "Saved game: " << game->GetName() << std::endl;
		}
	}
}

SaveThread::SaveThread()
{
	assert(!s_instance);
	s_instance = this;
	_worker = std::thread(&SaveThread::Go, this);
}

SaveThread::~SaveThread()
{
	_abort = true;
	_cv.notify_one();
	_worker.join();

	s_instance = nullptr;
}
