/***********************************************************************
 * Programmer	: Oscar Saavedra
 * Class	    : CECS 326-01
 * Due Date	    : November 5, 2022
 * Description	: This program makes use of the POSIX implementation of
 *  the Linux shared memory mechanism. This slave.c program receives its
 *  number and name of the shared memory segment via commandline
 *  arugments from master.c program. This program then outputs a message
 *  to identify itself. The program opens the existing shared memory 
 *  segment, acquires access to it, and write its child number into the
 *  next available slot in the shared memory. The pgrogram then closes
 *  the shared memory segment and terminates.
 ***********************************************************************/

#include "myShm.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>

int main(int argc, char** argv)
{
    // Create variables
    const int child_num = atoi(argv[0]);  // child number
    const char *shared_mem_name = argv[1];  // name of shared memory 
    const int SIZE = sizeof(struct SHARED_MEM_CLASS);  // size of struct object
    struct SHARED_MEM_CLASS *shared_mem_struct;  // structure of shared memory
    int shared_mem_fd;  // shared memory file descriptor
    int local_index;	// Local variable for index from shared memory

    // Print child number and shared memory name
    printf("Slave begins execution\n");
    printf("I am child number %d, received shared memory name %s\n", child_num, shared_mem_name);

    // Open the shared memory segment
    shared_mem_fd = shm_open(shared_mem_name, O_RDWR, 0666);
    if(shared_mem_fd == -1)
    {
        printf("\nSlave %d: Shared memory failed: %s\n", child_num, strerror(errno));
        exit(1);
    }
    else
    {
        // Map the shared memory segment
        shared_mem_struct = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED,
                                shared_mem_fd, 0);
        if(shared_mem_struct == MAP_FAILED)
        {
        printf("\nSlave %d: Map failed: %s\n", child_num, strerror(errno));
            /* close and shm_unlink */
            exit(1);
        }
        else
        {   
	    // Copy shared memory index to local variable index
	    local_index = shared_mem_struct->index;
	    printf("\n\nIndex pulled from shared memory: %d\n\n", local_index);
            // Write to the shared memory segment
            shared_mem_struct->response[shared_mem_struct->index] = child_num;
            shared_mem_struct->index += 1;
            printf("I have written my child number [%d] to shared memory\n", child_num);
        }
        
        // Unmap the shared memory structure
        if(munmap(shared_mem_struct, SIZE) == -1)
        {
            printf("\nSlave %d: Unmap failed: %s\n", child_num, strerror(errno));
            exit(1);
        }

        // Close the shared memory segment
        if (close(shared_mem_fd) == -1) 
        {
            printf("\nSlave %d: Close failed: %s\n", child_num, strerror(errno));
            exit(1);
        }
        else
            printf("Slave closes access to shared memory and terminates\n");
    }

    return 0;
}
