#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <assert.h>
#include <stdarg.h>

#include <pthread.h>
#include <unistd.h>


static void * thread_fn_callback (void *arg)
{
    char *input = (char*)arg;
    
    char *string = "Hello World";
    while (1)
    {
    	printf("String: %s \n", input);	
        sleep(1);
    }
}

void thread1_create()
{
    pthread_t   pthread1;           // Thread Handler

    static char *thread1_input1 = "I am thread 1";

    int rc = pthread_create(&pthread1,                  // Thread creation: Fork Point
                        NULL, 
                        thread_fn_callback, 
                        (void*) thread1_input1 );       // This should be memory in Heap or a static variable

    if (rc != 0)            				// On success, returns 0, else -1 on failure
    {
        printf("Error Occurred: %d\n", rc);
        exit(0);
    }
}



int main()
{
    thread1_create();
    printf("main is paused\n");
    //pause();
    pthread_exit(0);
    printf("Exiting Main thread \n");
    //while(1);
}
