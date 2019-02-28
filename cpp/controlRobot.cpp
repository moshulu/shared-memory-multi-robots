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

	int shm_fd;
	void *ptr;

	struct message{
		char *robot;
		char *movement;
		char *speed;
	};

	struct message m;
	char *robot1;
	char *movement1;
	char *speed1;

	printf("type a robot id (undertaker, kane): ");
	scanf("%s", &robot1);

	printf("type a robot movement (f, r, l, b): ");
	scanf("%s", &movement1);

	printf("type a robot speed (0 - 100): ");
	scanf("%s", &speed1);
	
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

	sprintf((char*)ptr, "%s %s %s", &robot1, &movement1, &speed1);

	printf("your message has been written to shared memory. \n");
	printf("	content: %s | %s | %s\n", &robot1, &movement1, &speed1);
	return 0;

}
