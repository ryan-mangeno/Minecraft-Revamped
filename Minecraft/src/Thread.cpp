#include "Thread.h"

namespace Thread {

	ThreadPool::ThreadPool(size_t num_threads)
		: m_stop_pool(false)
	{

		for (size_t i = 0; i < num_threads; ++i) {
			m_workers.emplace_back(&ThreadPool::worker_function, this);
		}

	}

	ThreadPool::~ThreadPool()
	{
		stop();
	}

	void ThreadPool::enqueue_task(Task task)
	{
		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			m_tasks.push(task);
		}
		m_condition.notify_one();
	}

	void ThreadPool::stop()
	{
		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			m_stop_pool = true;
		}

		m_condition.notify_all(); // Notify all workers to stop


		for (std::thread& worker : m_workers) {
			worker.join();

		}
	}


	void ThreadPool::worker_function()
	{
		while (true) {
			Task t;

			{
				std::unique_lock<std::mutex> lock(m_queue_mutex);

				// wait for a task or the stop signal
				m_condition.wait(lock, [this] {
					return this->m_stop_pool || !this->m_tasks.empty();
					});

				if (this->m_stop_pool && this->m_tasks.empty())
					return;

				t = std::move(this->m_tasks.front());
				this->m_tasks.pop();
			}

			// execute the task outside the lock to minimize lock duration
			t();
		}
	}

}
