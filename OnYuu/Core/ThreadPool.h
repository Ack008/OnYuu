#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <vector>
#include <stdexcept>

namespace OnYuu {

    class ThreadPool {
    public:
        // Crea il pool con n thread (default: numero di core logici)
        explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());

        // Non copiabile, non spostabile
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        ~ThreadPool();

        // Invia un task al pool, ritorna un future con il risultato
        template<typename F, typename... Args>
        auto submit(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

        // Aspetta che tutti i task in coda siano completati
        void waitAll();

        // Numero di thread nel pool
        size_t size() const { return workers_.size(); }

        // Numero di task in attesa
        size_t pendingTasks() const;

		// chiude il pool: non accetta nuovi task, aspetta quelli in coda e poi termina i thread
		void shutdown();

    private:
        std::vector<std::thread>          workers_;
        std::queue<std::function<void()>> tasks_;

        mutable std::mutex      mutex_;
        std::condition_variable taskCondition_;
        std::condition_variable doneCondition_;

        bool     stop_ = false;
        uint32_t activeTasks_ = 0; // task attualmente in esecuzione

        void workerLoop();
    };

    // ============================================================================
    // TEMPLATE IMPLEMENTATION
    // ============================================================================

    template<typename F, typename... Args>
    auto ThreadPool::submit(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        if (stop_) {
            throw std::runtime_error("ThreadPool: submit called after shutdown");
        }

        // Incapsula il task in un packaged_task per ottenere il future
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );

        std::future<ReturnType> future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push([task]() { (*task)(); });
        }

        taskCondition_.notify_one();
        return future;
    }

} // namespace OnYuu