#ifndef __MYTHREADPOOL_H__
#define __MYTHREADPOOL_H__




#include <future>
#include <queue>
#include <functional>


class MyThreadPool
{
public:
    using Task = std::packaged_task<void()>;

    // Some data associated to each thread.
    struct ThreadData
    {
        int                     id;         // Could use thread::id, but this is filled before the thread is started
        int                     nThreads;   // Total number of threads.
        std::thread             t;          // The thread object
        std::queue<Task>        tasks;      // The job queue
        std::condition_variable cv;         // The condition variable to wait for threads
        std::mutex              m;          // Mutex used for avoiding data races
        bool                    stop;       // When set, this flag tells the thread that it should exit
    };

private:
    // The thread function executed by each thread
    static inline void threadFunc( ThreadData* pData )
    {
        std::unique_lock<std::mutex> l( pData->m, std::defer_lock );
        while( true )
        {
            l.lock();

            // Wait until the queue won't be empty or stop is signaled
            pData->cv.wait(l, [pData] ()
            {
                return (pData->stop || !pData->tasks.empty()); 
            } );

            // Stop was signaled, let's exit the thread
            if (pData->stop) { return; }

            // Pop one task from the queue...
            Task task = std::move( pData->tasks.front() );
            pData->tasks.pop();

            l.unlock();

            // Execute the task!
            task();
        }
    }

    int                                 nThreads;
    ThreadData*                         threads;
    std::vector< std::future<void> >    futures;

public:
    inline MyThreadPool()
    {
        nThreads = (int) std::thread::hardware_concurrency();
		threads = new ThreadData [ nThreads ];

        for( int i=0; i<nThreads; ++i )
        {
            threads[i].stop = false;
            threads[i].id = i;
            threads[i].nThreads = nThreads;
            threads[i].t = std::thread( threadFunc, threads+i );
        }
    }

    template <typename TFunc>
    inline void AppendTask( const TFunc& func )
    {
        for( int i=0; i<nThreads; ++i )
        {
            // Function that creates a simple task
            Task task( std::bind( func, threads+i ) );

            futures.push_back( task.get_future() );

            std::unique_lock<std::mutex> l( threads[i].m );
            threads[i].tasks.push( std::move(task) );

            // Notify the thread that there is work do to...
            threads[i].cv.notify_one();
        }

        // Wait for all the tasks to be completed...
        for( auto& f : futures )
            f.wait();
        futures.clear();
    }

    inline ~MyThreadPool()
    {
        // Send stop signal to all threads and join them...
        for( int i=0; i<nThreads; ++i )
        {
            std::unique_lock<std::mutex> l( threads[i].m );
            threads[i].stop = true;
            threads[i].cv.notify_one();
        }

        // Join all the threads
        for( int i=0; i<nThreads; ++i )
            threads[i].t.join();

        delete[] threads;
    }
};




#endif //__THREADPOOL_H__
