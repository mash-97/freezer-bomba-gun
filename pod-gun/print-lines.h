#ifndef stdio
  #include <stdio.h>
#endif
#ifndef stdlib
  #include <stdlib.h>
#endif 
#ifndef unistd
  #include <unistd.h>
#endif

#define PRINT_LINES_SIZE 10
char *PRINT_LINES[PRINT_LINES_SIZE];

void __ALLOCATE_PRINT__(int indx, int size) {
	if(PRINT_LINES[indx]!=NULL)
		free(PRINT_LINES[indx]);
	PRINT_LINES[indx] = (char *)malloc(sizeof(char)*size);
}

void __INIT_PRINT_LINES__(void) {
	for(int i=0; i<PRINT_LINES_SIZE; i++)
		PRINT_LINES[i] = (char *)malloc(sizeof(char)*50);
}

void __PRINT_LINES__(int size) {
	printf("\r");
	for(int i=1; i<=size; i++)
		printf("-- %s --", PRINT_LINES[i]);
	fflush(stdout);
}