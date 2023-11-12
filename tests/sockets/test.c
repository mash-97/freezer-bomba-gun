#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


void *p(void *x) {
	printf("\n");
	for(int i=0; i<10000000*3; i++)
	{
		sleep(0.5);
		printf("# %lu => %s: %d\n", pthread_self(), (char *)x, i);
		fflush(stdout);
	}
	return NULL;
}

void *input(void *x) {
	char inp[1024];
	while(1) {
		printf("%lu: Enter your input: ", pthread_self());
		scanf(" %[^\n]", inp);
		printf("%lu: Your input: %s, len: %lu\n", pthread_self(), inp, strlen(inp));
	}
	return NULL;
}

int main() {
	int i =0;
	printf("current i is 0\n");
	for(int x=0; x<100000*3; x++);
	//printf("\f");
	pthread_t t1, t2, t3;
	pthread_create(&t1, NULL, p, "Hello World");
	pthread_create(&t2, NULL, p, "Life is short!");
	pthread_create(&t3, NULL, input, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	printf("\n");
	return 0;
}
