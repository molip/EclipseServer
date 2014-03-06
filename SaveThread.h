#pragma once

#include <condition_variable>
#include <thread>
#include <deque>

class LiveGame;

class SaveThread
{
public:
	SaveThread();
	~SaveThread();

	static SaveThread* Instance() { return s_instance; }

	void Push(const LiveGame& game);
	const LiveGame* Pop();

	void Go();

private:
	std::mutex _queueMutex, _cvMutex;
	std::deque<const LiveGame*> _queue;
	std::condition_variable _cv;
	bool _abort;
	std::thread _worker;
	static SaveThread* s_instance;
};
