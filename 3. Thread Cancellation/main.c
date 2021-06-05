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



void cleanUP_MemoryHandler (void *args);
void cleanUP_FileHandler(void *file);
void* th1(void *args);
void* th2(void *args);


pthread_t thread1;
pthread_t thread2;



void cleanUP_MemoryHandler (void *args)
{
    printf("%s invoked ... \n", __FUNCTION__);
    free(args);
}

void cleanUP_FileHandler(void *file)
{
    printf("%s invoked ... \n", __FUNCTION__);
    fclose((FILE *)file);
}


void* th1(void *args)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    pthread_cleanup_push(cleanUP_MemoryHandler, args);
    
    FILE *fptr = fopen("myfile.txt", "a+");
    if (!fptr )
    {
        printf("Cannot open:  errno: %d \n", errno);
        pthread_exit(0);
    }

    pthread_cleanup_push(cleanUP_FileHandler, fptr);

    int len = 0;
    char str[100];
    int counts = 0;

    while (1)
    {
        len = sprintf(str, "%d: I am thread %d \n", counts++, *(int*)args );
        fwrite(str, sizeof(char), len, fptr);
        fflush(fptr);  
        sleep(1); 
    }

    fclose(fptr);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
}

void* th2(void *args)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_cleanup_push(cleanUP_MemoryHandler, args);

    FILE *fptr = fopen("myfile2.txt", "a+");
    if (!fptr )
    {
        printf("Cannot open:  errno: %d \n", errno);
        pthread_exit(0);
    }
    pthread_cleanup_push(cleanUP_FileHandler, fptr);
    int len = 0;
    char str[100];
    int counts = 0;
    while (1)
    {
        len = sprintf(str, "%d: I am thread %d \n", counts++, *(int*)args );
        fwrite(str, sizeof(char), len, fptr);
        fflush(fptr);
        sleep(1); 
    }
    printf("Thread got cancelled");
    fclose(fptr);

    pthread_cleanup_pop(2);
    pthread_cleanup_pop(1);
}

int main()
{
    int *num = (int*)calloc(1, sizeof(int));
    *num = 1;
    int *threadMem1 = (int*)calloc (1, sizeof(int));
    int *threadMem2 = (int*)calloc (1, sizeof(int));
    *threadMem1 = 1;
    *threadMem2 = 2;
    pthread_create(&thread1, PTHREAD_CREATE_JOINABLE, th1, (void*) (threadMem1));
    pthread_create(&thread2, PTHREAD_CREATE_JOINABLE, th2, (void*) (threadMem2));

    pthread_join(thread1,NULL); 
    while(1);

}
