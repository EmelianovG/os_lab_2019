/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *  Modified by A.Kostin
 ********************************************************
 * mutex.c
 *
 * Simple multi-threaded example with a mutex lock.
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int common = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

void do_one_thing(int *pnum_times) {
    printf("Первая функция начала работу\n");
    pthread_mutex_lock(&mut);
    printf("Первая функция заблокировала первый мьютекс\n");
    sleep(1000);
    pthread_mutex_lock(&mut2);
	pthread_mutex_unlock(&mut);
	pthread_mutex_unlock(&mut2);
}

void do_another_thing(int *pnum_times) {
    printf("Вторая функция начала работу\n");
    pthread_mutex_lock(&mut2);
    printf("Вторая функция заблокировала второй мьютекс\n");
    sleep(1000);
    pthread_mutex_lock(&mut);
	pthread_mutex_unlock(&mut2);
    pthread_mutex_unlock(&mut);
}


int main() {
  pthread_t thread1, thread2;

  if (pthread_create(&thread1, NULL, (void *)do_one_thing,
			  (void *)&common) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_create(&thread2, NULL, (void *)do_another_thing,
                     (void *)&common) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }


  return 0;
}
