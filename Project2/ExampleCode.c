#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

//SIGNAL HANDLER
volatile int signo = 0;
    // marked volatile because it may change asynchronously
    // that is, signals may be received at any time


// very simple signal handler
// note that it returns normally, so we can only use it with non-error signals
void handler(int signum)
{
    signo = signum;
}

// a very simple exit handler
void make_a_note(void)
{
    puts("We are in the exit handler");
}

int main(int argc, char **argv)
{
    // register an exit handler: make_a_note will be called after main returns
    atexit(make_a_note);
    
    // register some signal handlers
    // we can reuse handler because it will receive the signal
    signal(SIGHUP, handler);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGCONT, handler);

    pause();  // stops the process until a signal is received
    // alternative:
    // while (signo == 0) { puts("Waiting"); sleep(1); }

    if (signo > 0) psignal(signo, "caught signal");
    
    return EXIT_SUCCESS;
} // END SIGNAL HANDLER EXAMPLE

// THREADS

// arguments of pthread_create
int   // returns 0 if successful, or an error code (does not set errno)
    pthread_create(
        pthread_t *tid, // id of new thread will be written here
        pthread_attr_t *attrs,  // ptr to struct requesting features
            // or NULL to get the default attributes
        void *(*thread_function)(void *),
            // function that the thread will execute
        void *args  // argument to pass to thread_function
    );

//starting a thread
pthread_t tid;
struct arg_t *args = ....;
err = pthread_create(&tid, NULL, thread_fun, args);
if (err != 0) { errno = err; perror("pthread_create"); exit(EXIT_FAILURE); }
// at this point, the thread has started and tid contains its thread id
// do other things

// arguments of pthread_join
int   // returns 0 for success, error number on error
    pthread_join(
        pthread_t tid,   // id of the thread we want to join (wait for)
        void **retval    // where to write the void * that the thread returned
                        // or NULL to ignore the return value
    )

struct retval_t *ret;
pthread_join(tid, &ret);
// at this point, the thread has stopped and ret points to its return value
// note: we passed &ret so that join could change what ret points to

/*when we join a thread
    - join will block until the specified thread has ended
    - join returns immediately if the thread has already ended*/

// EXAMPLE OF SPLITTING TASKS INTO MULTIPLE THREADS
// very simple example of splitting a task into multiple threads
// note: code to check errors not included

struct arg {
    int length;
    double *data;
};


// thread function
void *compute_sum(void *argptr)
{
    struct arg *args = (struct arg *) argptr;  // assume argptr has correct type
    
    int i;
    double sum = 0.0;
    for (i = 0; i < args->length; i++) {
        sum += args->data[i];
    }
    
    // must return a pointer
    double *retval = malloc(sizeof(double));
    
    *retval = sum;
    
    return retval;
}

// driver function

#define THREADS 5

void compute_sums(double *array, int length)
{
    pthread_t tids[THREADS];   // hold thread ids
    struct arg args[THREADS];  // hold arguments
    double *retval, sum = 0.0;
    int i, start, end;
    
    // initialize all arguments
    // e.g., we could break an array into equal-sized chunks
    end = 0;
    for (i = 0; i < THREADS; i++) {
        start = end;
        end = length * (i + 1) / THREADS;
        args[i].length = end - start;
        args[i].data = &array[start];
    }
    
    // start all threads
    for (i = 0; i < THREADS; i++) {
        pthread_create(&tids[i], NULL, compute_sum, &args[i]);
    }
    
    // we now have THREADS+1 threads running
    
    // wait for all threads to finish
    for (i = 0; i < THREADS; i++) {
        pthread_join(tids[i], &retval);
            // (only) current thread waits until tid[i] has finished
            // retval will point to the sum
        sum += *retval;
        free(retval);
    }
} // END EXAMPLE

// LOCKS
 int lock; // global variable
        
        void lock() {
            int prev = test_and_set(lock, 1);  // example; not a real function
                // sets lock to 1
                // prev has previous value of lock
            
            // if it was already locked, we need to wait until someone else unlocks
            while (prev == 1) {
                prev = test_and_set(lock, 1);
            }
            
            // once we get here, we know that we closed the lock
        
        }
        
            // only safe to call this if we hold the lock
        void unlock() {
            lock = 0;
        }// END EXAMPLE

//Use mutex to create a lock
    pthread_mutex_t lock;   // a struct or something (abstract)
    
    pthread_mutex_init(&lock, NULL);  // initialize lock
        // must be called exactly once before the lock can be used
    
    pthread_mutex_lock(&lock);  // acquire lock; block until lock becomes available
    
    pthread_mutex_unlock(&lock);  // release lock
    
    pthread_mutex_t balance_lock = PTHREAD_MUTEX_INITIALIZER;
        // global variable; already initialized

    //Thread 1:
        pthread_mutex_lock(&balance_lock);
        bank_balance += 100;
        pthread_mutex_unlock(&balance_lock);
    
    //Thread 2:
        pthread_mutex_lock(&balance_lock);
        bank_balance -= 50;
        pthread_mutex_unlock(&balance_lock);

//Lock arguments
int pthread_mutex_init(pthread_mutex_t *mut, pthread_mutex_attr_t *attr); // Init DOES NOT allocate space
int pthread_mutex_lock(pthread_mutex_t *mut);
int pthread_mutex_unlock(pthread_mutex_t *mut);
int pthread_mutex_destroy(pthread_mutex_t *mut);

   /*We always pass a pointer to the mutex object
        -> duplicating the object has undefined result
        -> we don't ever assign to a pthread_mutex_t
    These all return 0 on success, non-0 on failure*/
/*
Initialize once
lock before entering the mutually exclusive part of your code
unlock after exiting the mutually exclusive part of your code
Destroy when you no longer need the lock*/