#include <cstring>
#include "inc/PtzProtocolHandler.h"
#include "inc/EPtzCmdType.h"

#define ADDRESS 0x01
using namespace std;

#define HOME_PAN 1800
#define HOME_TILT 0

PtzCmdMapping gPtzCmdMapping[PTZ_CMD_NUM]={
    {
        .cmd = 0x00,
        .ptzCmdType = EPtzCmdType::PTZ_STOP,
    },
    {
        .cmd = 0x02,
        .ptzCmdType = EPtzCmdType::PTZ_PAN_RIGHT,
    },
    {
        .cmd = 0x04,
        .ptzCmdType = EPtzCmdType::PTZ_PAN_LEFT,
    },
    {
        .cmd = 0x08,
        .ptzCmdType = EPtzCmdType::PTZ_TILT_UP,
    },
    {
        .cmd = 0x10,
        .ptzCmdType = EPtzCmdType::PTZ_TILT_DOWN,
    },
        {
        .cmd = 0x51,
        .ptzCmdType = EPtzCmdType::PTZ_QUREY_PAN,
    },
        {
        .cmd = 0x53,
        .ptzCmdType = EPtzCmdType::PTZ_QUERY_TILT,
    },
    {
        .cmd = 0x8D,
        .ptzCmdType = EPtzCmdType::PTZ_QUERY_ZOOM,
    },
        {
        .cmd = 0x8E,
        .ptzCmdType = EPtzCmdType::PTZ_QUERY_FOCUS,
    },
        {
        .cmd = 0x59,
        .ptzCmdType = EPtzCmdType::PTZ_QUERY_PAN_RES,
    },
        {
        .cmd = 0x5B,
        .ptzCmdType = EPtzCmdType::PTZ_QUERY_TILT_RES,
    },
};

EPtzCmdType PtzProtocolHandler::findPtzCmdType(unsigned char cmd){

    for(int i=0;i<PTZ_CMD_NUM; i++){

        if( gPtzCmdMapping[i].cmd == cmd)
            return gPtzCmdMapping[i].ptzCmdType;
    }

    return EPtzCmdType::PTZ_UNKNOWN;
}

int PtzProtocolHandler::init(){
    int error;
    bSerialBeOpened = false;
    packetParser = PacketParser();
    queueControl = TDQueue();
    queueBuffer = new char[QUEUE_SIZE];
    readBuffer = new char[MAX_MSG_SIZE];
    queueCS = new APP_UTIL::AppCS();
    queueControl.DQueueInit(&readQueue, queueBuffer, QUEUE_SIZE) ;
    serialPort = new SerialPort("/dev/ttyUSB0", BaudRate::B_9600);
    error = pthread_create(&ptzReceiverThread, NULL, _ThreadReceiver, this);
    if(error != 0){
        printf("Can't create _ThreadReceiver!\r\n");
        return -1;
    }
    error = pthread_create(&ptzParserThread, NULL, _ThreadParser, this);
    if(error != 0){
        printf("Can't create _ThreadParser!\r\n");
        return -1;
    }
    return 1;
}

void PtzProtocolHandler::setHandler(ProtocolHandleInf *handle){

    cbHandler = handle;
}

int PtzProtocolHandler::open(){

    return openPTZcom();
}

int PtzProtocolHandler::close(){

    return closePTZcom();
}

int PtzProtocolHandler::openPTZcom(){
	serialPort->SetTimeout(-1); // Block when reading until any data is received
	serialPort->Open();
    bSerialBeOpened = true;
    return 1;
} 

int PtzProtocolHandler::closePTZcom(){
    bAlways = false;
    serialPort->Close();
    queueControl.DQueueClose(&readQueue);
    return 1;
}  


int PtzProtocolHandler::sendRawdata(char *data, int size){

    serialPort->Write(data, size);
    return 1;
}

void PtzProtocolHandler::parseCmd(char *data, int size){

    unsigned char head = data[0];
    EPtzCmdType cmdType;
    if(head == 0xFF){

        cmdType = findPtzCmdType(data[3]);

    }else if(head == 0x53){

        cmdType = findPtzCmdType(data[1]);
    }
    

    if( cbHandler != NULL)
        cbHandler->received(cmdType, readBuffer, size);
}

void PtzProtocolHandler::runParserThread(){

    printf("runParserThread start.\r\n");
    int size = 0;
    
    while(bAlways){
        usleep(20*1000);
        APP_UTIL::AppMutex((APP_UTIL::AppCS*) queueCS);
        
        while( (size = packetParser.queuePacketParser(&queueControl, &readQueue, readBuffer, MAX_MSG_SIZE)) > 0) {

            parseCmd(readBuffer, size);

        }//while

    }//end while

}//end runParserThread

void PtzProtocolHandler::runReceiverThread(){

    printf("runReceiverThread start.\r\n");

    while( !bSerialBeOpened)
        usleep(20*1000);

    while(bAlways){
 
        std::string readData;
	    serialPort->Read(readData);
        pushToQueue((char *)readData.c_str(), readData.length());

    }//end 

}//end runReceiverThread

int PtzProtocolHandler::pushToQueue(char *data, const int size){

    //printf("got data size=[%d][%02X]\r\n", size, data[0]);
    APP_UTIL::AppMutex((APP_UTIL::AppCS*) queueCS);
    if(queueControl.DQueueAddItems(&readQueue, data , size) <= 0 ){

        return -1;
    }
    return 1;
}

HomeCoordinate_S PtzProtocolHandler::getHomeCoordinate(){

    return homeCoordinate;
}

void PtzProtocolHandler::ptzHome(){
    
    unsigned char data[9] = {0xFF, ADDRESS, 0x40, 0x53};

    data[4] = homeCoordinate.panH;//0x09;
    data[5] = homeCoordinate.panL;//0x93;
    data[6] = homeCoordinate.tiltH;//0x00;
    data[7] = homeCoordinate.tiltL;//0x68;
    data[8] = packetParser.checksum(data+1, 7);

    sendRawdata((char *)data, 9);
}

void PtzProtocolHandler::ptzStop(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x00, 0x00, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzLeft(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x04, 0x20, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzRight(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x02, 0x20, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzUp(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x08, 0x00, 0x20};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzDown(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x10, 0x00, 0x20};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzAutoFocus(){

    unsigned char data[MAX_MSG_SIZE] = {0x53, 0x03, 0x01, 0x00, 0x01, 0x01};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data, MAX_MSG_SIZE-1);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzZoomNear(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x20, 0x00, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzZoomFar(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x40, 0x00, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzQueryPan(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x51, 0x00, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzQueryTilt(){
    unsigned char data[MAX_MSG_SIZE] = {0xFF, ADDRESS, 0x00, 0x53, 0x00, 0x00};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data+1, MAX_MSG_SIZE-2);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzQueryZoom(){
    unsigned char data[MAX_MSG_SIZE] = {0x53, 0x8D, 0x00, 0x00, 0x00, 0x01};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data, MAX_MSG_SIZE-1);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}

void PtzProtocolHandler::ptzQueryFocus(){
    unsigned char data[MAX_MSG_SIZE] = {0x53, 0x8E, 0x00, 0x00, 0x00, 0x01};
    data[MAX_MSG_SIZE-1] = packetParser.checksum(data, MAX_MSG_SIZE-1);
    
    sendRawdata((char *)data, MAX_MSG_SIZE);
}


void PtzProtocolHandler::ptzSetPanTiltPosition(int pan, int tilt){
    
    unsigned char data[9] = {0xFF, ADDRESS, 0x40, 0x53};
    
    data[4] = (char)((0xFF00&pan)>>8);
    data[5] = (char)(0x00FF&pan);
    data[6] = (char)((0xFF00&tilt)>>8);
    data[7] = (char)(0x00FF&tilt);
    data[8] = packetParser.checksum(data+1, 7);

    sendRawdata((char *)data, 9);
}

void PtzProtocolHandler::ptzSetZoomPosition(int zoom){
    unsigned char data[14] = {0x53, 0x8D, ADDRESS};
    
    data[3] = (char)0x00;//wide - 0x00, near - 0x01
    data[4] = (char)0x00;//wide - 0x00, near - 0xC8
    data[5] = ADDRESS;
    data[6] = packetParser.checksum(data, 6);
    data[7] = 0x53;
    data[8] = 0x8E;
    data[9] = ADDRESS;
    data[10] = (char)0x01;//need to modify 
    data[11] = (char)0x36;//need to modify 
    data[12] = ADDRESS;
    data[13] = packetParser.checksum(data+7, 6);

    sendRawdata((char *)data, 14);
}


