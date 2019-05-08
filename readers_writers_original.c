#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <limits.h>
#include <semaphore.h>
#include <time.h>
#include "check.h"
#include <syscall.h>

/* The mutex lock for the writer*/
pthread_mutex_t rw_mutex;
/* mutual exclusion to update a variable (read_count --> current number
of readers*/
pthread_mutex_t mutex;

/* current number of readers */
int read_count=0;
/*shared data*/
int shared_data=1;

pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *writer(void *param);
void *reader(void *param);

void *reader(void *param) {
   while(1) {
		/***Atualiza: read_count***/
        pthread_mutex_lock(&mutex);
		read_count++;
		/***O primeiro leitor bloqueia os escritores***/
		if(read_count==1)
	      pthread_mutex_lock(&rw_mutex);
        pthread_mutex_unlock(&mutex);
		/****LEITURA DO DADO****/
		printf("\n(%07d) -- read: %05d",syscall(SYS_gettid),shared_data);
        sleep(1);
		/***Atualiza: read_count***/
        pthread_mutex_lock(&mutex);
		read_count--;
		//O último leitor libera os escritores.
		if(read_count==0)
            pthread_mutex_unlock(&rw_mutex);
        pthread_mutex_unlock(&mutex);
		//sleep(rand()%10+1);
		sleep(rand()%3+1);
   }
}
void *writer(void *param) {
	sleep(1);
   while(1) {
      pthread_mutex_lock(&rw_mutex);
		shared_data++;
		printf("\n(%07d) -- wrote: %05d",syscall(SYS_gettid),shared_data);
      pthread_mutex_unlock(&rw_mutex);
		sleep(1);
   }
}

int main(int argc, char *argv[]) {
	srand(time(0));
   /* Verify the correct number of arguments were passed in */
   if(argc != 4) {
      fprintf(stderr, "USAGE:./main.out <INT> <INT> <INT>\n");
		exit(1);
   }

   int mainSleepTime = atoi(argv[1]); /* Time in seconds for main to sleep */
   int numWriter = atoi(argv[2]); /* Number of writers*/
   int numReader = atoi(argv[3]); /* Number of readers*/

   /* Initialize the app */
   pthread_mutex_init(&rw_mutex, NULL);
   pthread_mutex_init(&mutex, NULL);
   read_count = 0;

   pthread_t * thread  =  malloc ((numWriter+numReader)*sizeof(pthread_t));
   /* Create the writer threads */
	int i;
   for(i = 0; i < numWriter; i++) {
      /* Create the thread */
      thread[i]=pthread_create(&tid,&attr,writer,NULL);
    }

   /* Create the reader threads */
   for(i = 0; i < numReader; i++) {
      /* Create the thread */
      thread[numReader+i]=pthread_create(&tid,&attr,reader,NULL);
   }

   /* Sleep for the specified amount of time in milliseconds */
   sleep(mainSleepTime);

   /* Exit the program */
   printf("\nExit the program\n");
   exit(0);
}

