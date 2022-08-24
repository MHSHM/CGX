#pragma once

#include "Task.h"

#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>


#define MAX_THREADS 12

class ThreadPool
{
public:
	ThreadPool() = default;

public:
	void Initialize(uint32_t N);

	void Schedule(Task&& task);
	void ThreadLoop();
	void Shutdown();

private:
	uint32_t m_maxThreads = 0;
	uint32_t m_avaialableThreads = 0;

	std::vector<std::thread> m_threads;
	std::queue<Task> m_tasks;
	mutable std::mutex m_lock;
	std::condition_variable m_cond;

	bool m_stop = false;
};