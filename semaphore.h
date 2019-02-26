#pragma once

#include <mutex>
#include <condition_variable>

class semaphore
{
public:
	explicit semaphore(unsigned int count = 0) noexcept
	: m_count(count)
	{}

	void post() noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			++m_count;
		}
		m_cv.notify_one();
	}

	void post(unsigned int count) noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			m_count += count;
		}
		m_cv.notify_all();
	}

	void wait() noexcept
	{
		std::unique_lock lock(m_mutex);
		m_cv.wait(lock, [&]() { return m_count != 0; });
		--m_count;
	}

	template<typename T>
	bool wait_for(T&& t) noexcept
	{
		std::unique_lock lock(m_mutex);
		auto result = m_cv.wait_for(lock, t, [&]() { return m_count != 0; });
		if(result == false) return false;
		--m_count;
		return true;
	}

	template<typename T>
	bool wait_until(T&& t) noexcept
	{
		std::unique_lock lock(m_mutex);
		auto result = m_cv.wait_until(lock, t, [&]() { return m_count != 0; });
		if(result == false) return false;
		--m_count;
		return true;
	}

private:
	unsigned int m_count;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};
