#ifndef WAV_STREAM_H
#define WAV_STREAM_H

#define PAYLOAD_SIZE 160

typedef unsigned char byte;

struct rtp_conn{
	char *ip;
	int port;
};

struct thread_info{
	char threadNumber;
	pthread_t tid;
	char status;
	struct rtp_conn *rtpConn;
	unsigned char *wav;
	struct thread_info *prev;
	struct thread_info *next;
};

#endif WAV_STREAM_H