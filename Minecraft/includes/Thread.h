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
		ThreadPool(size_t numThreads);
		~ThreadPool();

		void stop();
		void enqueueTask(Task);

	private:

		void workerFunction();

		std::vector<std::thread> m_Workers;
		std::queue<Task> m_Tasks;
		std::mutex m_QueueMutex;
		std::condition_variable m_Condition;
		bool m_StopPool;

	};



}

#endif //
