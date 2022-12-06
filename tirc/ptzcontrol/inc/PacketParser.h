#ifndef __PACKET_PARSER_H__
#define __PACKET_PARSER_H__
#include <stdio.h>
//#include <stdlib.h>
#include "TDQueue.h"

#define QUEUE_SIZE 1024
#define MAX_MSG_SIZE 7

class PacketParser{

public:
    PacketParser(){;}
    ~PacketParser(){;}
    unsigned char checksum(unsigned char *data, int size);
    int queuePacketParser(TDQueue *queueControl, DQueue *readQueue, char *outBuf, int maxBufSize);
    int packMsg(char *out, char *data, int size);
};

#endif //__PACKET_PARSER_H__