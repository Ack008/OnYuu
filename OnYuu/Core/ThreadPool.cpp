#include "ThreadPool.h"

namespace OnYuu {

    // ============================================================================
    // CONSTRUCTOR & DESTRUCTOR
    // ============================================================================

    ThreadPool::ThreadPool(size_t threadCount) {
        if (threadCount == 0) {
            threadCount = 1;
        }

        workers_.reserve(threadCount);

        for (size_t i = 0; i < threadCount; i++) {
            workers_.emplace_back(&ThreadPool::workerLoop, this);
        }
    }

    ThreadPool::~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }

        // Sveglia tutti i thread in modo che possano uscire dal loop
        taskCondition_.notify_all();

        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    // ============================================================================
    // PUBLIC METHODS
    // ============================================================================

    void ThreadPool::waitAll() {
        std::unique_lock<std::mutex> lock(mutex_);

        // Aspetta finché non ci sono task in coda E nessun task in esecuzione
        doneCondition_.wait(lock, [this]() {
            return tasks_.empty() && activeTasks_ == 0;
            });
    }

    size_t ThreadPool::pendingTasks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }

    // ============================================================================
    // PRIVATE METHODS
    // ============================================================================

    void ThreadPool::workerLoop() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                // Dorme finché c'è lavoro oppure è arrivato lo shutdown
                taskCondition_.wait(lock, [this]() {
                    return !tasks_.empty() || stop_;
                    });

                // Condizione di uscita: shutdown richiesto e coda vuota
                if (stop_ && tasks_.empty()) {
                    return;
                }

                // Prende il prossimo task dalla coda
                task = std::move(tasks_.front());
                tasks_.pop();
                activeTasks_++;
            }

            // Esegue il task FUORI dal lock — altri thread possono lavorare
            task();

            {
                std::lock_guard<std::mutex> lock(mutex_);
                activeTasks_--;

                // Notifica waitAll() se non c'è più niente da fare
                if (tasks_.empty() && activeTasks_ == 0) {
                    doneCondition_.notify_all();
                }
            }
        }
    }

	void ThreadPool::shutdown() {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			stop_ = true;
		}
		taskCondition_.notify_all();
	}

} // namespace OnYuu