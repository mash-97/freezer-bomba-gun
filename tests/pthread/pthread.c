#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void *func(void *args) {
	unsigned long tid = pthread_self();
	float seconds = 2.0f;
	printf("#tid: %lu -> Going to sleep for %.3f\n", tid, seconds);
	sleep(seconds);
	printf("#tid: %lu -> Awaken after sleep of %.3f seconds\n", tid, seconds);
	pthread_cancel(tid);
	return NULL;
}

void jj(void *j) {
	int i = *(int *)j;
	void *x = &i;
	i = (int)(*((unsigned long *)x));
	printf("=> %d\n", i);
}

int main(void) {
	unsigned long x=5;
	jj(&x);
	time_t st = time(NULL);
	unsigned long tid = pthread_self();
	printf("main#tid: %lu\n", tid);

	pthread_t ptid;
	if(!pthread_create(&ptid, NULL, func, &tid)) 
		printf("main#tid: %lu -> Peer thread created ptid: %lu\n", tid, ptid);
	else
		printf("main#tid: %lu -> Failed to create thread\n", tid);

	int rc=-1;
	sleep(5);
	time_t et = time(NULL);
	double td = difftime(et, st);
	printf("Total Time: => %.4lf\nrc: %d\n", td, rc);
	return 0;
}
