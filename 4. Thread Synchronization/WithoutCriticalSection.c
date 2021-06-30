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

pthread_t readThread;
pthread_t writeThread;


typedef struct __dll__
{
    int data;
    struct __dll__ *next;
}dll_t;
dll_t *actualList;

void createList(dll_t *actual, int data);
int number = 0;

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
    printf("The Data in the List are: ");
    while (actual)
    {
        printf("%d ", actual->data);
        actual = actual->next;
    }
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

            if (matchFound)     printf("Number found at position: %d\n", position);
            else                printf("No Match Found\n");
            dummy = actualList; 
            number = 0;
        }
        sleep(1);
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
                tail = dummy;
                
            }
	    else
	    {		
            while ( dummy )
            {
                if (number == dummy->data)
                {
                    tail->next = dummy->next;
                    dummy->data = 0;
                    dummy->next = 0;
                    printf("Node deleted\n");
                    
                    break;
                }
                tail = tail->next;
                dummy = dummy->next;
            } 
	    }
            dummy = actualList;
            tail = actualList;
            number = 0;
        }
        sleep(1);
    }
}

int main()
{
    actualList = (dll_t *)calloc (1, sizeof (dll_t));
    int data[] = {1,2,3,4,5,6,7,8,9,10};

    for (int i = 0; i< sizeof(data)/ sizeof(int); i++)
    {
        createList(actualList, data[i]);
    }

    displayList(actualList);

    pthread_create(&readThread, PTHREAD_CANCEL_DEFERRED, thread_toRead, (void *)actualList);
    pthread_create(&writeThread, PTHREAD_CANCEL_DEFERRED, thread_toWrite, (void *)actualList);

    while (1)
    {
        printf("Which number do you want to read [1-10]? : ");
        scanf ("%d", &number);
        sleep(1);
        displayList(actualList);
    }
}
