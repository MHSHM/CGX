#pragma once

#include <thread>
#include <future>

class Task
{
public:

	Task(std::function<void()>&& _func) :
		func(std::move(_func))
	{

	}

	Task(const Task&) = delete;

	Task(Task&& other) noexcept
	{
		func = std::move(other.func);
	}

	void operator()()
	{
		func();
	}

	inline std::future<void> GetFuture() { return func.get_future(); }

private:
	std::packaged_task<void()> func;
};