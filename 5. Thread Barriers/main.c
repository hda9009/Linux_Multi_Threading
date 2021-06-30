#include <memory.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <semaphore.h>

#define true 1
#define false 0

typedef struct __dll__
{
    int data;
    struct __dll__ *next;
}dll_t;

typedef struct __barrier__
{   
    uint32_t thresholdCountThread;
    uint32_t currentWaitCounts;
    pthread_cond_t cv;
    pthread_mutex_t mutex;
    uint8_t isReadyAgain;
    pthread_cond_t busyCV;          // Block Additional Threads
}__barrier__t;

dll_t *actualList;
pthread_t readThread;
pthread_t writeThread;
pthread_t addThread;
pthread_mutex_t protect_dll;

int i;
int number = 0;
void createList(dll_t *actual, int data);



// pthread_barrier_t Barrier;       // for inbuild Library
static __barrier__t Barrier;
void threadBarrierInit(__barrier__t *barrier, uint32_t threshold);
void threadBarrierWait(__barrier__t *barrier);
void threadBarrierDestroy(__barrier__t *barrier);
void sharedResource(pthread_barrier_t *barrier);


void threadBarrierInit(__barrier__t *barrier, uint32_t threshold)
{
    barrier->isReadyAgain = true;
    barrier->currentWaitCounts = 0;
    barrier->thresholdCountThread = threshold;

    pthread_mutex_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->busyCV, NULL);
}

void threadBarrierWait(__barrier__t *barrier)
{
    // pthread_barrier_wait();
    // pthread_barrier_destroy();


    pthread_mutex_lock(&barrier->mutex);

    while(!barrier->isReadyAgain)
    {
        pthread_cond_wait(&barrier->busyCV, &barrier->mutex);
    }

    if (barrier->currentWaitCounts + 1 == barrier->thresholdCountThread)
    {
        barrier->isReadyAgain = false;
        pthread_cond_signal(&barrier->cv);
        pthread_mutex_unlock(&barrier->mutex);
        return;
    }

    barrier->currentWaitCounts++;
    pthread_cond_wait(&barrier->cv, &barrier->mutex);
    
    barrier->currentWaitCounts--;
    if (! barrier->currentWaitCounts)
    {
        barrier->isReadyAgain = true;
        pthread_cond_broadcast(&barrier->busyCV);
    }
    else
    {
        pthread_cond_signal(&barrier->cv);
    }

    pthread_mutex_unlock(&barrier->mutex);

}


void threadBarrierDestroy(__barrier__t *barrier)
{
    pthread_cond_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->busyCV);
    pthread_mutex_destroy(&barrier->mutex);
}


void createList(dll_t *actual, int data)
{
    static dll_t *temp;
    static int count = 0;
    if(!count)
    {
        actual->data = data;
        actual->next = 0;
        temp = (dll_t *)calloc (1, sizeof (dll_t));
        temp = actual;
        count++;
    }
    else
    {
        dll_t *dummy = (dll_t *)calloc (1, sizeof (dll_t));
        dummy->data = data;
        dummy->next = NULL;
        temp->next = dummy;
        temp = dummy;
        dummy = 0;
        free(dummy);
    }
}

void displayList(dll_t *actual)
{
    // pthread_mutex_lock(&protect_dll);
    printf("The Data in the List are: ");
    while (actual)
    {
        printf("%d ", actual->data);
        actual = actual->next;
    }
    // pthread_mutex_unlock(&protect_dll);
    printf("\n");
}

void * thread_toRead(void *param)
{
    dll_t *dummy = (dll_t *)calloc (1, sizeof (dll_t));
    dummy = actualList;
    while(1)
    {
        
        if (number >= 1 && number <= 10)
        {
            int i = 0;
            // printf("Read Thread will get lock: %d\n", i++);
            // pthread_mutex_lock(&protect_dll);
            // sleep(5);
            int position = 0;
            int matchFound = 0;

            while ( dummy )
            {
                position++;
                if (number == dummy->data)
                {
                    matchFound = 1;
                    break;
                }
                dummy = dummy->next;
            } 

            if (matchFound)     printf("%d: Number found at position: %d\n", i++, position);
            else                printf("%d: No Match Found\n", i++);
            dummy = actualList; 

            // pthread_mutex_unlock(&protect_dll);
            // sleep(1);
            sharedResource(&Barrier);
        }
    }
}

void *thread_toWrite(void *param)
{
    dll_t *dummy = (dll_t *)calloc (1, sizeof (dll_t));
    dll_t *tail = (dll_t *)calloc (1, sizeof (dll_t));
    dummy = actualList;
    tail = actualList;

    while(1)
    {
        if (number >= 1 && number <= 10)
        {

            dummy = dummy->next;
            if (number == tail->data)
            {
                tail->data = 0;
                tail->next = 0;
            }

            // pthread_mutex_lock(&protect_dll);
            while ( dummy )
            {
                if (number == dummy->data)
                {
                    tail->next = dummy->next;
                    dummy->data = 0;
                    dummy->next = 0;
                    printf("%d: Node deleted\n", i++);
                    
                    break;
                }
                tail = tail->next;
                dummy = dummy->next;
            } 

            dummy = actualList;
            tail = actualList;

            // pthread_mutex_unlock(&protect_dll);
            // sleep(1);
            sharedResource(&Barrier);
        }
    }
}

void *thread_toAdd(void *param)
{
    dll_t *dummy = (dll_t *)calloc (1, sizeof (dll_t));
    dll_t *head = (dll_t *)calloc (1, sizeof (dll_t));
    head = actualList;

    while(1)
    {
        int sum = 0;
        if (number)
        {
            // int i = 0;
            // printf("Add Thread will get lock: %d\n", i++);
            // pthread_mutex_lock(&protect_dll);
            // sleep(10);
            while(head)
            {
                sum += head->data;
                head = head->next;
            } 

            printf ("%d: The sum of all the data in the Linked List: %d\n", i++, sum);
            head = actualList;
            // pthread_mutex_unlock(&protect_dll);

            // sleep(1);
            sharedResource(&Barrier);
        }
    }  
}


void sharedResource(pthread_barrier_t *barrier)
{
    // pthread_barrier_wait(&barrier);  // for inbuild Library
    threadBarrierWait(&Barrier);
    number = 0;

    static int i;
    if (i++ > 2)
    {
        pthread_kill(addThread, NULL);
        pthread_barrier_init(&Barrier, NULL, 2);
    }
    else if (i++ > 4)
    {
        pthread_create(&writeThread, PTHREAD_CANCEL_DEFERRED, thread_toWrite, (void *)actualList);
        pthread_barrier_init(&Barrier, NULL, 3);
    }
}

int main()
{
    #if 1
    actualList = (dll_t *)calloc (1, sizeof (dll_t));
    int data[] = {1,2,3,4,5,6,7,8,9,10};

    for (int i = 0; i< sizeof(data)/ sizeof(int); i++)
    {
        createList(actualList, data[i]);
    }

    displayList(actualList);

    threadBarrierInit(&Barrier, 3);

    pthread_barrier_init(&Barrier, NULL, 3);
    pthread_create(&readThread, PTHREAD_CANCEL_DEFERRED, thread_toRead, (void *)actualList);
    pthread_create(&writeThread, PTHREAD_CANCEL_DEFERRED, thread_toWrite, (void *)actualList);
    pthread_create(&addThread, PTHREAD_CANCEL_DEFERRED, thread_toAdd, (void *)actualList);


    // pthread_mutex_init(&protect_dll, NULL);
    // if (pthread_mutex_lock(&protect_dll ) == 0)      printf("Mutex Locked\n");
    // else                                        printf("Mutex is not Locked\n");
    // if (!pthread_mutex_lock(&protect_dll) == 0)      printf("Mutex Locked\n");
    // else                                        printf("Mutex is not Locked\n");
    // if (!pthread_mutex_unlock(&protect_dll) == 0)      printf("Mutex UnLocked\n");
    // else                                        printf("Mutex is not UnLocked\n");
    // if (!pthread_mutex_unlock(&protect_dll) == 0)      printf("Mutex UnLocked\n");
    // else                                        printf("Mutex is not UnLocked\n");


    while (1)
    {
        printf("Which number do you want to read [1-10]? : ");
        i = 1;
        scanf ("%d", &number);
        printf("\n");
        sleep(1);
        displayList(actualList);
    }

    #endif
    
}

