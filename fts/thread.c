#include <pthread.h>
#include <fts/fts.h>
#include <fts/thread.h>


int fts_thread_manager_create(fts_thread_worker_t* thread_worker,
			      void* thread_arg,
			      int* thread_id)
{
    /*
      Default thread attribute:
      - joinable (not detached)
      - default (non real-time) scheduling policy
    */
    *thread_id = pthread_create(&thread_worker->thread, NULL, 
			       thread_worker->thread_function,
			       thread_arg);
    if (0 != *thread_id)
    {
	fprintf(stderr,"[fts_thread_manager] error while creating thread \n");
    }
    
    return *thread_id;
}



int fts_thread_manager_cancel(fts_thread_worker_t* thread_worker)
{
    int success;
    success = pthread_cancel(thread_worker->thread);
    if (0 != success)
    {
	fprintf(stderr, "[fts_thread_manager_cancel] no such thread found\n");
    }
    return success;
}
