#include <stdio.h>
#include <stdlib.h>

#include "wav_stream.h"

int read_exact(byte *buf, int len);
int read_cmd(byte *buf);
int write_exact(byte *buf, int len);
int write_cmd(byte *buf, int len);

int read_exact(byte *buf, int len){
  int i, got=0;
  
  do {
    if ((i = read(0, buf+got, len-got)) <= 0){
      return(i);
	}
    got += i;
  } while (got<len);
  
  return(len);
}

int write_exact(byte *buf, int len){
  int i, wrote = 0;

  do {
    if ((i = write(1, buf+wrote, len-wrote)) <= 0){
      return (i);
	}
    wrote += i;
  } while (wrote<len);

  return (len);
}

int read_cmd(byte *buf){
  int len;

  if (read_exact(buf, 2) != 2){
    return(-1);
  }
  len = (buf[0] << 8) | buf[1];
  return read_exact(buf, len);
}

int write_cmd(byte *buf, int len){
  byte li;

  li = (len >> 8) & 0xff;
  write_exact(&li, 1);
  
  li = len & 0xff;
  write_exact(&li, 1);

  return write_exact(buf, len);
}

int decode_cmd(unsigned char *readBuffer, struct rtp_conn *rtpConn){
	if(strstr(readBuffer, "sendwav") != NULL){
		int length = strlen(readBuffer+9) + 1;
		rtpConn->ip = (unsigned char *)malloc(length);
		memcpy(&(rtpConn->port), readBuffer+7, 2);
		strcpy(rtpConn->ip, readBuffer+9);
		//memcpy(rtpConn->ip, readBuffer+7, strlen(readBuffer+7)+1);
		return 0;
	}else{
		return -1;
	}
}