#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "wav_stream.h"

//#define PAYLOAD_SIZE 160

struct timespec req = {0};

int stream(int udpSocket, struct sockaddr_in *si_other, unsigned char *wav){
	
	//FILE *file_pointer;
	//char file_name[] = "/home/aryan/Desktop/tmp/test.wav";
	long fileOffset = 0;
	long fileSize = malloc_usable_size(wav);
	char play;
	
	unsigned char headerPart_11 = 128;	//V,P,X,CC
	unsigned char headerPart_12 = 8;	//M,PT
	unsigned short seqNumber = 0;
	unsigned short seqNumNBO;
	unsigned int timestamp = 0;
	unsigned int timestampNBO;		//Network Byte Order :: MSB (Big Endian)
	unsigned int ssrc;
	unsigned char rtpPacket[172];
	unsigned int increment;
	//unsigned int readBytes;
	int milisec = 20; // length of time to sleep, in miliseconds
	
	req.tv_sec = 0;
	req.tv_nsec = milisec * 1000000L;
	ssrc = htonl((unsigned int)time(NULL));

	//if((file_pointer = fopen(file_name, "rb")) == NULL){
	//	return -1;
	//}
	
	do{
		increment = 0;
		memcpy(rtpPacket, &headerPart_11, sizeof(headerPart_11));	//start : byte_1
		increment += sizeof(headerPart_11);
		memcpy(rtpPacket+increment, &headerPart_12, sizeof(headerPart_12));	//start : byte_2
		increment += sizeof(headerPart_12);
		seqNumber++;
		seqNumNBO = htons(seqNumber);
		memcpy(rtpPacket+increment, &seqNumNBO, sizeof(seqNumNBO));		//start : byte_3
		increment += sizeof(seqNumber);
		timestampNBO = htonl(timestamp);
		timestamp += 160;
		memcpy(rtpPacket+increment, &timestampNBO, sizeof(timestampNBO));		//start : byte_5
		increment += sizeof(timestampNBO);
		memcpy(rtpPacket+increment, &ssrc, sizeof(ssrc));
		increment += sizeof(ssrc);
		//readBytes = fread(rtpPacket+increment, 1, PAYLOAD_SIZE, file_pointer);	//start : byte_13
		//printf("\npacket size : %i", sizeof(rtpPacket));
		
		if((fileSize-fileOffset-1) > PAYLOAD_SIZE){
			memcpy(rtpPacket+increment, wav+fileOffset, PAYLOAD_SIZE);
			fileOffset += PAYLOAD_SIZE;
			play = 1;
		}else{
			memcpy(rtpPacket+increment, wav+fileOffset, fileSize-fileOffset-1);
			//fileOffset += PAYLOAD_SIZE;
			play = 0;
		}
		
		if (sendto(udpSocket, rtpPacket, 172, 0, (struct sockaddr*) si_other, sizeof(*si_other)) == -1){
            return -1;
		}
		nanosleep(&req, (struct timespec *)NULL);
	//}while(readBytes==PAYLOAD_SIZE);
	}while(play);
	//fclose(file_pointer);
	return 0;
}


// TODO:: here..pass by value/reference?
int createSocket(int *udpSocket, struct sockaddr_in *si_me, struct sockaddr_in *si_other, struct rtp_conn *rtpConn){
	*udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*udpSocket == -1){
		return -1;
    }
	
	 //zero out the structure
    memset((char *) si_me, 0, sizeof(*si_me));
	
    si_me->sin_family = AF_INET;
    si_me->sin_port = htons(10000);
    si_me->sin_addr.s_addr = htonl(INADDR_ANY);
	
	si_other->sin_family = AF_INET;
	//si_other->sin_port = htons(5062);
	si_other->sin_port = rtpConn->port;
	//si_other->sin_addr.s_addr = inet_addr("192.168.8.101");
	si_other->sin_addr.s_addr = inet_addr((const char *)(rtpConn->ip));
	memset(si_other->sin_zero, '\0', sizeof (si_other->sin_zero)); 
    //bind socket to port
    //if( bind(*udpSocket, (struct sockaddr*)si_me, sizeof(*si_me) ) == -1){
    //    return -1;
    //}else{
	//	return 0;
	//}
	return 0;
}

//=========================================================================================================================
//|
//| 			 0                   1                   2                   3
//| 			 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//| 			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| 			|V=2|P|X|  CC   |M|     PT      |       sequence number         |			payload type :: aLaw
//| 			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| 			|1 0|0|0|0 0 0 0|0|0 0 0 1 0 0 0|0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0|
//| 			+---+-+-+-------+-+-------------+-------------------------------+
//|
//=========================================================================================================================