#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

int main() {
	const char *name = "/my_shm";
	const int SIZE = 4096;
	int shm_fd;
	void *ptr;
	/* open the ddshared memory segment */
	shm_fd = shm_open(name, O_RDONLY, 0666);
	if (shm_fd == -1) {
		perror("shared memory is empty. continually trying to open...");
		while(shm_fd == -1){
			sleep(2);
			perror("trying to open...");
			shm_fd = shm_open(name, O_RDONLY, 0666);
		}
	}
	
	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		perror("in mmap()");
		exit(1);
	}

	/* now read from the shared memory region... */
	printf("Content in the shared memory:");
	printf("%s\n", ptr);

	/* ...and copy into a local string. */
	char str[100];
	strcpy(str, ptr);

	if(strcmp(ptr, "") == 0){
		perror("currently in the process of input!\n");
		while(strcmp(ptr, "") == 0){
			printf("waiting to complete input...\n");
			printf("ptr: %s\n", ptr);
			ptr = mmap(0,SIZE,PROT_READ,MAP_SHARED, shm_fd, 0);
			if(strcmp(ptr,"") != 0){
				break;
			}
			sleep(5);
		}
	}
	printf("ptr: %s\n",ptr);

	/* split the str string into tokens, and assign them to variables */
	char * pch;
	char * robot;
	char * movement;
	char * speed;
	int iteration = 0;

	printf("splitting string \"%s\" into tokens:\n", ptr);
	pch = strtok(str, " ");

	/* get the first 3 arguments only, and put it in the variables. */
	while(pch != NULL) {
		printf("%s\n", pch);
		if(iteration == 0){
			robot = pch;
		}

		if(iteration == 1){
			movement = pch;
		}

		if(iteration == 2){
			speed = pch;
		}
		pch = strtok(NULL, " ");
		iteration = iteration + 1;
	}

	/* check to see if this robot name is the correct name */
	if(strcmp("kane", robot) != 0){
		printf("the command wasn't for this robot.\n");
	} else {
		/* if it is the correct name, clear the shared memory to get the next command ready */
		shm_unlink("/my_shm");
		/* do robot stuff with provided variables now! */
		printf("robot: %s\n", robot);
		printf("movement: %s\n", movement);
		printf("speed: %s\n\n", speed);

		if(strcmp("f",movement) == 0){
			printf("move forward\n");
		} else if(strcmp("b", movement) == 0){
			printf("move backwards\n");
		} else if(strcmp("l", movement) == 0){
			printf("move left\n");
		} else if(strcmp("r", movement) == 0){
			printf("move right\n");
		} else {
			printf("not a valid movement\n");
			return -1;
		}

		int speed_numeric = atoi(speed);
		if(speed_numeric < 0 || speed_numeric > 100){
			printf("not a valid speed");
			return -1;
		} else{
			printf("setting speed to %d\n", speed_numeric);
		}
	}


	return 0;
}
