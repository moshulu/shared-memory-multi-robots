# shared-memory-multi-robots
Using shared memory to control 2 robots in linux based systems.

## File structure
The file labelled "examples" are the documents written by Dr. Jiacun Wang, and used as a basis of how to write to and read from memory. They are not used in the final product.

The file labelled "c" contain files written in c, that the author practiced writing to and from with, along with splitting strings.

The file labelled "cpp" contains the final product. They were adapted into C++ because Amigobots require a C++ library.

## How to run

1. Clone the repository.

2. Direct your command line to the cpp directory.

3. Run ./controlRobot 		<-- this file sets the commands that the robots run

4. Run ./kane 			<-- this file reads commands for robots labelled "kane"

5. Run ./undertaker 		<-- this file reads commands for robots labelled "undertaker"

You can hypothetically run all 3 programs at the same time.

## Major constraints

According to [this documentation on StackOverflow](https://stackoverflow.com/questions/23793992/do-i-need-to-use-shm-unlink-on-a-shared-memory-object), it seems like there is memory leakage concerns when it comes to manually manipulating memory space (!). When you call...

```

shm_unlink(shared-memory-address-name)

```

...it unlinks successfully from memory, but you *must* close the operating process in order for it to be cleared from the local cache. It is for this reason that **the current version does not have real time looking at the shared memory space for new commands after one has been executed**.

Trust me, I spent many hours trying to figure this out.

Right now, ./kane and ./undertaker look for the shared memory space when the program is initially run. If nothing's there, it waits until something is there. If something is there, it executes it, clears the memory, and exits the respective program.
