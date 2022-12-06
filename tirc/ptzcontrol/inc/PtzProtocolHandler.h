#ifndef __PTZ_PROTOCOL_HANDLER_H__
#define __PTZ_PROTOCOL_HANDLER_H__
#include <stdio.h>
#include <stdlib.h>
#include "TDQueue.h"
#include "PacketParser.h"
#include <pthread.h>
#include <unistd.h>
#include "../CppLinuxSerial/SerialPort.hpp"
#include "TDQueue.h"
#include "PlatformThread.h"
#include "ProtocolHandleInf.h"

using namespace mn::CppLinuxSerial;
using namespace APP_UTIL;

typedef struct _HomeCoordinate_S{

    unsigned char panH = 0x09;
    unsigned char panL = 0x32;  //0x32 0x93
    unsigned char tiltH = 0x00;
    unsigned char tiltL = 0x00;

}HomeCoordinate_S;


class PtzProtocolHandler{

    PacketParser packetParser;
    pthread_t ptzParserThread;
    pthread_t ptzReceiverThread;
    TDQueue queueControl;
    DQueue readQueue;
    char *queueBuffer;//for TDQueue
    char *readBuffer;//for TDQueue
    AppCS *queueCS;
    SerialPort *serialPort;
    bool bAlways = true;
    ProtocolHandleInf *cbHandler;
    bool bSerialBeOpened;
    /*method*/
    int openPTZcom();
    int closePTZcom();
    int pushToQueue(char *data, const int size);
    int queuePacketParser(DQueue *readQueue, char *outBuf, int maxBufSize);
    void parseCmd(char *data, int size);
    HomeCoordinate_S homeCoordinate;
public:
    PtzProtocolHandler(){;}
    ~PtzProtocolHandler(){;}
    void setHandler(ProtocolHandleInf *handle);
    int init();
    int open();
    int close();
    int sendRawdata(char *oridata, int size);
    EPtzCmdType findPtzCmdType(unsigned char cmd);
    void runReceiverThread();
    void runParserThread();
    static void *_ThreadParser(void *arg){
        ((PtzProtocolHandler *)arg)->runParserThread();
        return ((void*)0);
    }
    static void *_ThreadReceiver(void *arg){
        ((PtzProtocolHandler *)arg)->runReceiverThread();
        return ((void*)0);
    }

    HomeCoordinate_S getHomeCoordinate();

    void ptzHome();

    void ptzStop();
    void ptzLeft();
    void ptzRight();
    void ptzUp();
    void ptzDown();

    void ptzSetPanTiltPosition(int pan, int tilt);
    void ptzSetZoomPosition(int zoom);

    void ptzAutoFocus();
    void ptzZoomNear();
    void ptzZoomFar();

    void ptzQueryPan();
    void ptzQueryTilt();
    void ptzQueryZoom();
    void ptzQueryFocus();

};

#endif //__PTZ_PROTOCOL_HANDLER_H__
