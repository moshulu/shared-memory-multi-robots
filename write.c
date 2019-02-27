#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>

int main(){
	const int SHM_SIZE = 4096;
	const int MSG_SIZE = 100;
	const char *name = "/my_shm";
	char message[MSG_SIZE];

	int shm_fd;
	void *ptr;

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	if(shm_fd == -1){
		perror("In shm_open()");
		exit(1);
	}

	ftruncate(shm_fd, SHM_SIZE);

	ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if(ptr == MAP_FAILED){
		printf("Map failed\n");
		return -1;
	}
	printf("type a message: ");
	fgets(message, MSG_SIZE, stdin);

	sprintf(ptr, "%s", message);

	printf("your message has been written to shared memory. \n");
	printf("	content: %s\n", message);
	return 0;

}
