//All my includes and defines are in my main.h header
#include "main.h"


void *myThreadFun(void *vargp)
{
    sleep(2);
    printf("Printing GeeksQuiz from Thread \n");
    return NULL;
}

int main(int argc, char **argv)
{
    parseOptions(argc, argv);
    parsePassengerSchedules();



    pthread_t thread_id;
    printf("Before Thread\n");
    pthread_create(&thread_id, NULL, myThreadFun, NULL);
    pthread_join(thread_id, NULL);
    printf("After Thread\n");
    exit(0);
}