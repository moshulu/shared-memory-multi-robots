#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include "Aria.h"

int main(int argc, char **argv) {
	const char *name = "/my_shm";
	const int SIZE = 4096;
	int shm_fd;
	void *ptr;
	
	while(1){
		/* open the ddshared memory segment */
		if (!shm_open(name, O_RDONLY, 0666)) {
			perror("shared memory is empty. continually trying to open...");
			while(shm_fd == -1){
				sleep(2);
				perror("trying to open...");
				if(shm_open(name, O_RDONLY, 0666)){
					shm_unlink(name);
					break;
				} else{
					shm_unlink(name);
				}
			}
		}
		shm_fd = shm_open(name, O_RDONLY, 0666);
		
		/* now map the shared memory segment in the address space of the process */
		ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
		if (ptr == MAP_FAILED) {
			perror("in mmap()");
			sleep(2);
			continue;
		}
	
		/* now read from the shared memory region... */
		printf("Content in the shared memory:");
		printf("%s\n", (char *)ptr);

		/* ...and copy into a local string. */
		char str[100];
		strcpy(str, (char *)ptr);

		if(strcmp((char *)ptr, "") == 0){
			perror("currently in the process of input!\n");
			while(strcmp((char *)ptr, "") == 0){
				printf("waiting to complete input...\n");
				printf("ptr: %s\n", (char *)ptr);
				ptr = mmap(0,SIZE,PROT_READ,MAP_SHARED, shm_fd, 0);
				if(strcmp((char *)ptr,"") != 0){
					break;
				}
				sleep(5);
			}
		}
		printf("ptr: %s\n",(char *)ptr);

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
		if(strcmp("undertaker", robot) != 0){
			printf("the command wasn't for this robot.\n");
			continue;
		} else {
			/* if it is the correct name, clear the shared memory to get the next command ready */
			shm_unlink("/my_shm");
			/* do robot stuff with provided variables now! */
			printf("robot: %s\n", robot);
			printf("movement: %s\n", movement);
			printf("speed: %s\n\n", speed);
			
			int speed_numeric = atoi(speed);
			if(speed_numeric < 0 || speed_numeric > 100){
				printf("not a valid speed");
				return -1;
			} else{
				printf("setting speed to %d\n", speed_numeric);
			}

			/* initialize Aria necessary methods */
			Aria::init();
			ArArgumentParser parser(&argc, argv);
			parser.addDefaultArgument("-rh 10.0.126.18");
			parser.loadDefaultArguments();
			ArRobot myRobot;
			ArSonarDevice sonar;
			ArActionDesired myDesired;

			ArRobotConnector robotConnector(&parser, &myRobot);
			if(!robotConnector.connectRobot()){
				printf("could not connect to robot");
				return -1;
			}

			printf("\n\nconnected to robot \"undertaker\"\n\n");

			myRobot.runAsync(true);
			myRobot.enableMotors();

			if(strcmp("f",movement) == 0){
				printf("move forward\n");
				myRobot.setVel((double)speed_numeric);
			} else if(strcmp("b", movement) == 0){
				printf("move backwards\n");
				speed_numeric = 0 - speed_numeric;
				myRobot.setVel((double)speed_numeric);
			} else if(strcmp("r", movement) == 0){
				printf("move right\n");
				speed_numeric = 0 - speed_numeric;
				myRobot.lock();
				myRobot.setRotVel((double)speed_numeric);
				myRobot.unlock();
			} else if(strcmp("l", movement) == 0){
				printf("move left\n");
				myRobot.lock();
				myRobot.setRotVel((double)speed_numeric);
				myRobot.unlock();
			} else {
				printf("not a valid movement\n");
				return -1;
			}

			ArUtil::sleep(5000);
			myRobot.stopRunning();
			
			pch = "";
			robot = "";
			movement = "";
			speed = "";
		}
	}

	Aria::shutdown();
	return 0;
}
