#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "wav_stream.h"

void thread_handler(struct thread_info *threadInfo){
	struct timespec req = {0};
	int milisec = 1; // length of time to sleep, in miliseconds
	req.tv_sec = 0;
	req.tv_nsec = milisec * 1000000L;
	struct thread_info *current;
	current = threadInfo;
	while(1){
		if((current->next) != NULL){
			if(((current->status) <= 0) && ((current->threadNumber) > 1)){
				(current->prev)->next = current->next;
				(current->next)->prev = current->prev;
				free(current);
			}else{
				current = current->next;
			}
		}else{
			current = threadInfo;
		}
		nanosleep(&req, (struct timespec *)NULL);
	}
}