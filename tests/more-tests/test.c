#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

void countNum(char *str);

void countNum(char *str) {
	static int c=0;
	printf("(%s)=> c: %d\n", str, c++);
}

typedef  struct A {
	void *cart[3];
	void (* count)(char *str);
} A;

A *newA() {
	A *a = (A *)malloc(sizeof(A));
	a->count = countNum;
	return a;
}

typedef struct B {
	int x;
	void (* count)(char *str);
} B;

B *newB(int x) {
	B *b = (B *)malloc(sizeof(B));
	b->x = x;
	b->count = countNum;
	return b;
}

typedef struct C {
	unsigned long x;
	float y;
} C;

C *newC(unsigned long x, float y) {
	C *c = (C *)malloc(sizeof(C));
	c->x = x;
	c->y = y;
	return c;
}

char *P[10];

void *p1(void *a) {
	int v = *(int *)a; // *((int *)a);
	// printf("#(pid: %lu) v: %d\n", pthread_self(), v);
	if(P[v]!=NULL)
		free(P[v]);
	P[v] = (char *)malloc(sizeof(char)*30);
	for(int i=0; i<100000; i++) {
		sleep(1);
		sprintf(P[v], "(pid: %lu:%d): %d", pthread_self(), v, i);
	}
	return NULL;
}

void *p0(void *a) {
	int v = *((int *)a);
	//if(P[0]!=NULL)
	//	free(P[0]);
	// P[0] = (char *)malloc(sizeof(char)*10);
	// sprintf(P[0], "\r");
	printf("\r");
	for(int i=1; i<=v; i++)
		printf("-- %s -- ", P[i]);
	// printf("\n");
	fflush(stdout);
}


int main() {
//	srand(time(NULL));
//	printf("%d\n", rand());
//	for(int i=0; i<6; i++)	{
//		int a=rand();
//		char ca[20];
//		int b=rand();
//		sprintf(ca, "%d.%d", a, b);
//		printf("%f\n", atof(ca));
//	}
/*	A *a = newA();
	a->cart[0] = newB(3);
	a->cart[1] = newC(2,2);
	printf("cart0: %d\n", ((B *)a->cart[0])->x);
	printf("cart1: %lu\n", ((C *)a->cart[1])->x);
	free((B *)a->cart[0]);
	free((C *)a->cart[1]);*/
	for(int i=0; i<10; i++)
		P[i] = NULL;
/*	P[0] = (char *)malloc(sizeof(char)*10);
	sprintf(P[0], "hello world\n");
	P[1] = (char *)malloc(sizeof(char)*20);
	sprintf(P[1], "my input: %d\n", 3333);
	for(int i=0; i<2; i++)
		printf("%s", P[i]); */
//
//	pthread_t t[3];
//	int arg[3];
//
//	for(int i=0; i<3; i++) {
//		arg[i] = i+1;
//		pthread_create(&t[i], NULL, p1, &arg[i]);
//	}
//
//	int a=3;
//	printf("start tail: %d\n", a);
//	for(int i=0; i<10000; i++) {
//		sleep(2);
//		p0(&a);
//	}
	A *a = newA();
	B *b = newB(3);
	
	a->count("A");
	b->count("B");
	return 0;
}
