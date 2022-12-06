#include "inc/PacketParser.h"

unsigned char PacketParser::checksum(unsigned char *data, int size){

    unsigned char sum = 0;

    for(int i=0;i<size;i++)
        sum += (unsigned char)data[i];

    return (unsigned char)((0xFF)&sum);
}

int PacketParser::queuePacketParser(TDQueue *queueControl, DQueue *readQueue, char *outBuf, int maxBufSize){

    const int PACKET_SIZE = 7;
    int readSize, dataSize;
    while( (readSize = queueControl->DQueuePeekInQueues(readQueue, outBuf, maxBufSize)) >= PACKET_SIZE){

        if( (unsigned char) outBuf[0] != 0xFF && (unsigned char) outBuf[0] != 0x53  ){
            //printf("header not matched = [%02X]\r\n", (unsigned char) outBuf[0]);
            queueControl->DQueueRemoveItems(readQueue, NULL, 1);
            continue;
        }

        if( (unsigned char) outBuf[0] == 0xFF && (unsigned char) outBuf[1] != 0x01  ){
            //printf("header not matched data[1] = [%02X]\r\n", (unsigned char) outBuf[1]);
            queueControl->DQueueRemoveItems(readQueue, NULL, 1);
            continue;
        }

        unsigned char sum = 0;
        //if((unsigned char) outBuf[0] == 0xFF)
        //    sum = checksum( (unsigned char *)outBuf+1, PACKET_SIZE-2);
        //else
            sum = checksum( (unsigned char *)outBuf, PACKET_SIZE-1);

        if( (unsigned char) outBuf[PACKET_SIZE-1] != sum  ){
            printf("warn! checksum data = [%02X], sum=[%02X]\r\n", (unsigned char) outBuf[PACKET_SIZE-1], sum);
            queueControl->DQueueRemoveItems(readQueue, NULL, 1);
            continue;
        }

        queueControl->DQueueRemoveItems(readQueue, NULL, PACKET_SIZE);
        return PACKET_SIZE;    

    }//end while

    return 0;
}//end queuePacketParser

int PacketParser::packMsg(char *out, char *data, int size){


    return 1;
}
