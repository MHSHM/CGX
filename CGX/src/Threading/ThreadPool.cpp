#include "ThreadPool.h"

#include <utility>

void ThreadPool::Initialize(uint32_t N)
{
	m_maxThreads = N;
	m_avaialableThreads = std::min(m_maxThreads, std::thread::hardware_concurrency());
	m_avaialableThreads--;
	m_threads.reserve(m_avaialableThreads);

	for (int i = 0; i < m_avaialableThreads; ++i)
	{
		m_threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
	}
}

void ThreadPool::Schedule(Task&& task)
{
	std::lock_guard<std::mutex> lock(m_lock);
	m_tasks.push(std::move(task));
	m_cond.notify_one();
}

void ThreadPool::ThreadLoop()
{
	while (true)
	{
		// if the queue is empty sleep
		std::unique_lock<std::mutex> lock(m_lock);

		m_cond.wait(lock, [this]() {
			return !m_tasks.empty() || m_stop;
			});

		if (m_stop && m_tasks.empty())
		{
			return; 
		}

		Task task = std::move(m_tasks.front());
		m_tasks.pop();

		lock.unlock();

		task();
	}
}

void ThreadPool::Shutdown()
{
	m_stop = true;

	m_cond.notify_all();

	for (int i = 0; i < m_avaialableThreads; ++i)
	{
		m_threads[i].join();
	}
}