#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define MAXITEMS 100000
#define MAXTHREADS 30


int items; // read-only by producer and consumer
int buff[MAXITEMS];

struct {
  pthread_mutex_t mutex;
  int nput;       // next index to store 
  int nval;       // next value to store
} put = {
  PTHREAD_MUTEX_INITIALIZER
};

struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int nready;        // number ready for consumer
} nready = {
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
};


void *productor(void *arg)
{
  while (1)
    {
      pthread_mutex_lock(&put.mutex);
      if (put.nput >= items)
	{
	  pthread_mutex_unlock(&put.mutex);
	  return 0;
	}
      buff[put.nput] = put.nval;
      put.nput++;
      put.nval++;
      pthread_mutex_unlock(&put.mutex);

      pthread_mutex_lock(&nready.mutex);
      if (nready.nready == 0)
	pthread_cond_signal(&nready.cond);
      nready.nready++;
      pthread_mutex_unlock(&nready.mutex);

      *(int *)arg += 1;
    }
}

void *consumer(void *arg)
{
  for (int i = 0; i < items; ++i)
    {
      pthread_mutex_lock(&nready.mutex);
      while (nready.nready == 0)
	pthread_cond_wait(&nready.cond, &nready.mutex);

      nready.nready--;
      pthread_mutex_unlock(&nready.mutex);

      if (buff[i] != i)
	printf("buff[%d] = %d\n", i, buff[i]);
    }

  return 0;
}

int main(int argc, char *argv[])
{
  //  printf("%s:%d:%d\n", shared.buff, shared.nput, shared.nval);
  
  int i, threads, count[MAXTHREADS];
  pthread_t tid_produce[MAXTHREADS], tid_consume;

  items = MAXITEMS;
  threads = MAXTHREADS;

  //  pthread_setconcurrency(threads);

  // start producers
  for (i = 0; i < threads; i++)
    {
      count[i] = 0;
      pthread_create(&tid_produce[i], NULL, productor, &count[i]);
    }
  // wait for all producers terminate
  for (i = 0; i < threads; i++)
    {
      pthread_join(tid_produce[i], NULL);
      printf("count[%d] = %d\n", i, count[i]);
    }

  // start, then wait for the consumer thread
  pthread_create(&tid_consume, NULL, consumer, NULL);
  pthread_join(tid_consume, NULL);


  return 0;
}




