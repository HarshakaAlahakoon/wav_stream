#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "wav_stream.h"

short handle_req(struct thread_info *threadInfo);
void thread_handler(struct thread_info *threadInfo);
struct thread_info *get_last_threadInfo(struct thread_info *threads);

int main(int argc, char **argv){
	unsigned char  cmdReadBuffer[100];
	unsigned char  *cmdSendBuffer;
	cmdSendBuffer = (unsigned char  *)malloc(sizeof("start"));
	strcpy(cmdSendBuffer,"start");
	//write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
	
	FILE *file_pointer;
	char file_name[] = "/home/aryan/Desktop/tmp/test.wav";
	file_pointer = fopen(file_name, "rb");
	fseek(file_pointer, 0L, SEEK_END);
	long sz = ftell(file_pointer);
	rewind(file_pointer);
	//printf("\size : %d\n", sz);
	unsigned char *wavFile = (unsigned char *)malloc(sz);
	fread(wavFile, 1, sz, file_pointer);
	fclose(file_pointer);
	
	struct thread_info *threads;
	struct thread_info *current;
	threads = (struct thread_info *)malloc(sizeof(struct thread_info));
	threads->prev = NULL;
	threads->next = NULL;
	threads->status = 1;
	threads->threadNumber = 1;
//	pthread_create(&(threads->tid), NULL, thread_handler, threads);
	while (read_cmd(cmdReadBuffer) > 0) {
		cmdSendBuffer = realloc(cmdSendBuffer, strlen("received")+1);
		strcpy(cmdSendBuffer,"received");
		write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
		if(strstr(cmdReadBuffer, "sendwav")==NULL){
			cmdSendBuffer = realloc(cmdSendBuffer, strlen("Invalid Command")+1);
			strcpy(cmdSendBuffer,"Invalid Command");
			write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
		}else{
			cmdSendBuffer = realloc(cmdSendBuffer, strlen("new thread")+1);
			strcpy(cmdSendBuffer,"new thread");
			write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
			//pthread_create((pthread_t *)malloc(sizeof(pthread_t)), NULL, handle_req, NULL);
			current = get_last_threadInfo(threads);
			cmdSendBuffer = realloc(cmdSendBuffer, sizeof(current));
			strcpy(cmdSendBuffer,current);
			write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
			current->next = (struct thread_info *)malloc(sizeof(struct thread_info));
			(current->next)->prev = current;
			(current->next)->next = NULL;
			(current->next)->status = 1;
			(current->next)->threadNumber = (current->threadNumber) + 1;
			(current->next)->wav = malloc(sz);
			memcpy((current->next)->wav, wavFile, sz);
			//pthread_create(&((current->next)->tid), NULL, handle_req, current->next);
			pthread_create((pthread_t *)malloc(sizeof(pthread_t)), NULL, handle_req, current->next);
			cmdSendBuffer = realloc(cmdSendBuffer, strlen("thread created")+1);
			strcpy(cmdSendBuffer,"thread created");
			write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
		}
	}
//	free(&cmdReadBuffer);
//	free(cmdSendBuffer);
	return 0;
}	

short handle_req(struct thread_info *threadInfo){
	unsigned char  *cmdSendBuffer;
	cmdSendBuffer = malloc(strlen("child")+1);
	strcpy(cmdSendBuffer, "child");
	write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
	struct sockaddr_in si_me, si_other;
	int udpSocket;
	if (createSocket(&udpSocket, &si_me, &si_other) == -1){
		//cmdSendBuffer = (unsigned char  *)malloc(strlen("se1")+1);
		cmdSendBuffer = realloc(cmdSendBuffer, strlen("se1")+1);
		strcpy(cmdSendBuffer, "se1");
		write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
		return -1;
	}else{
		//cmdSendBuffer = (unsigned char  *)malloc(strlen("sok")+1);
		cmdSendBuffer = realloc(cmdSendBuffer, strlen("sok")+1);
		strcpy(cmdSendBuffer, "sok");
		write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
	}
	
	cmdSendBuffer = realloc(cmdSendBuffer, strlen("stream starting...")+1);
	strcpy(cmdSendBuffer, "stream starting...");
	write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
	if(stream(udpSocket, &si_other, threadInfo->wav) == -1){
		cmdSendBuffer = realloc(cmdSendBuffer, strlen("stream error")+1);
		strcpy(cmdSendBuffer, "stream error");
		write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
		return -1;
	}else{
		cmdSendBuffer = realloc(cmdSendBuffer, strlen("stream finished")+1);
		strcpy(cmdSendBuffer, "stream finished");
		write_cmd(cmdSendBuffer, strlen(cmdSendBuffer));
	}
	//free(cmdSendBuffer);
	//threadInfo->status = 0;
	close(udpSocket);
	return 0;
}

struct thread_info *get_last_threadInfo(struct thread_info *threads){
	struct thread_info *current;
	current = threads;
	do{
		if(current->next == NULL){
			return current;
		}else{
			current = current->next;
		}
	}while(current->next != NULL);
	return current;
}