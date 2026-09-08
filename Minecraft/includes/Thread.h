#ifndef THREAD_H
#define THREAD_H


#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <condition_variable>


namespace Thread {

	using Task = std::function<void()>;


	class ThreadPool {
	public:
		ThreadPool(size_t num_threads);
		~ThreadPool();

		void stop();
		void enqueue_task(Task);

	private:

		void worker_function();

		std::vector<std::thread> m_workers;
		std::queue<Task> m_tasks;
		std::mutex m_queue_mutex;
		std::condition_variable m_condition;
		bool m_stop_pool;

	};



}

#endif //
