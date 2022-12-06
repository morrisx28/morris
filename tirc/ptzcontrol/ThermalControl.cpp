#include "inc/ThermalControl.h"
#include "inc/TDQueue.h"
#include <string.h>
#include <cmath>

#define QUEUE_SIZE 1024
#define MAX_MSG_SIZE 7
#define DIFF_POS_TH 10
//using namespace APP_UTIL;

/* tilt -> -400: Max UP, 350: Max Down */
#define HOME_PAN 2353 //2353 2451
#define HOME_TILT 0

#define HomeCmdStatePanFinished 0x01
#define HomeCmdStateTiltFinished 0x02

ThermalControl::ThermalControl(/* args */){
    currPtzState = EPtzState::PTZ_IDLE;
    ptzProtocolHandler.setHandler(this);
    cmdCS = new APP_UTIL::AppCS();
    ptzControlCmdPos.posPan = 0;
    ptzControlCmdPos.posTilt = 0;
    ptzControlCmdPos.ePtzControlCmd = EPtzControlCmd::PTZ_CONTROL_IDLE;
}

ThermalControl::~ThermalControl(){
    
}

int ThermalControl::init(){

    return ptzProtocolHandler.init();
}

int ThermalControl::open(){

    currPtzPos.pan = -999;//wait update.
    currPtzPos.tilt = -999;//wait update.
    tempPtzPos.pan = -999;//double check if ready or not
    tempPtzPos.tilt = -999;//double check if ready or not

    ptzProtocolHandler.open();
    sleep(1);
    int cnt = 0;
    ptzready = 0;
    // while(cnt++ <5){
    while (!ptzready) {
        
        ptzQueryPan();
        usleep(200*1000);
        ptzQueryTilt();
        usleep(200*1000);

        if( currPtzPos.pan == -999 || currPtzPos.tilt == -999){
            continue;
        }
        else { 
            // first time get position
            if (tempPtzPos.pan != currPtzPos.pan || tempPtzPos.tilt != currPtzPos.tilt) {
                tempPtzPos.pan = currPtzPos.pan;
                tempPtzPos.tilt = currPtzPos.tilt;
                continue;
            }
            // double check if the position makes sense
            else if (tempPtzPos.pan == currPtzPos.pan || tempPtzPos.tilt == currPtzPos.tilt){
                ptzready = 1;
                break;
            }
        }
    }
    // if(cnt >=5){
    //     printf("Can't open Thermal PTZ\r\n");
    //     return -1;
    // }

    int error = pthread_create(&ptzCmdThread, NULL, _ThreadCmd, this);
    if(error != 0){
        printf("Can't create _ThreadParser!\r\n");
        return -1;
    }
    return 1;
} 

void ThermalControl::setPtzCmdEventHandler(PtzCmdEventInf *handler){
    cbHander = handler;
}

int ThermalControl::close(){

    return ptzProtocolHandler.close();
}

void ThermalControl::setPtzControlCmdPos(int pos, EPtzControlCmd ePtzControlCmd){

    ptzControlCmdPos.ePtzControlCmd = ePtzControlCmd;
    if(ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_PAN)
      ptzControlCmdPos.posPan = pos;
    else  if(ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_TILT)
      ptzControlCmdPos.posTilt = pos;
}

void ThermalControl::reset(){

    currPtzState = EPtzState::PTZ_IDLE;
}

int ThermalControl::received(EPtzCmdType type, char *data, int size){

    switch(type){
        case EPtzCmdType::PTZ_QUERY_PAN_RES:
            APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
            currPtzPos.pan = (int)((0xFF&data[4])<<8|(0xFF&data[5])); 
            // printf("Got PTZ_QUREY_PAN=[%d][%x][%x]\r\n", currPtzPos.pan, (int)(0xFF&data[4]), (int)(0xFF&data[5]));

            if(currPtzState != EPtzState::PTZ_BUSY)
                break;

            // printf("currPtzPos.pan= %d", currPtzPos.pan);
            // printf("\r\n");
            // printf("ptzControlCmdPos.posPan= %d", ptzControlCmdPos.posPan);
            // printf("\r\n");

            if( abs( currPtzPos.pan - ptzControlCmdPos.posPan)<= DIFF_POS_TH ){

                usleep(100*1000);

                if(ptzControlCmdPos.ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_HOMIMG){

                    fHomeCmdState |= HomeCmdStatePanFinished;
                    if( fHomeCmdState & HomeCmdStateTiltFinished){
                        currPtzState = EPtzState::PTZ_IDLE;

                        /* cb to Application. */
                        if( bCallBackEnable == true){
                            if(cbHander)
                                cbHander->received(EPtzControlCmd::PTZ_CONTROL_HOMIMG, 1);
                        }
                    }

                }else {

                    currPtzState = EPtzState::PTZ_IDLE;
                    /* cb to Application. */
                    if( bCallBackEnable == true){
                        if(cbHander)
                            cbHander->received(EPtzControlCmd::PTZ_CONTROL_PAN, 1);
                    }
                }
            }
        break;
        case EPtzCmdType::PTZ_QUERY_TILT_RES:
            APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
            currPtzPos.tilt = (short)((0xFF&data[4])<<8|(0xFF&data[5])); 
            // printf("Got PTZ_QUERY_TILT_RES=[%d][%d][%d][%x][%x]\r\n", currPtzPos.tilt,(short)((0xFF&data[4])<<8|(0xFF&data[5])), ptzControlCmdPos.posTilt, (int)(0xFF&data[4]), (int)(0xFF&data[5]));

            if(currPtzState != EPtzState::PTZ_BUSY)
                break;

            if( abs( currPtzPos.tilt - ptzControlCmdPos.posTilt)<= DIFF_POS_TH ){
                
                usleep(100*1000);

                if(ptzControlCmdPos.ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_HOMIMG){

                    fHomeCmdState |= HomeCmdStateTiltFinished;
                    if( fHomeCmdState & HomeCmdStatePanFinished){
                        currPtzState = EPtzState::PTZ_IDLE;
    
                        /* cb to Application. */
                        if( bCallBackEnable == true){
                            if(cbHander)
                                cbHander->received(EPtzControlCmd::PTZ_CONTROL_HOMIMG, 1);
                 
                        }
                    }

                }else {

                    currPtzState = EPtzState::PTZ_IDLE;
                    /* cb to Application. */
                    if( bCallBackEnable == true){
                        if(cbHander)
                            cbHander->received(EPtzControlCmd::PTZ_CONTROL_TILT, 1);
                    }
                }
            }
        break;
        case EPtzCmdType::PTZ_QUERY_ZOOM:

            printf("Got PTZ_QUERY_ZOOM=[%x][%x]\r\n", (int)(0xFF&data[4]), (int)(0xFF&data[5]));
        break;
        case EPtzCmdType::PTZ_QUERY_FOCUS:

            printf("Got PTZ_QUERY_FOCUS=[%x][%x]\r\n", (int)(0xFF&data[4]), (int)(0xFF&data[5]));
        break;
        default:
        break;
    }

    return 1;
}

void ThermalControl::runCmdThread(){

    printf("runCmdThread start.\r\n");
    int size = 0;
    
    while(1){
        usleep(100*1000);
        if(currPtzState != EPtzState::PTZ_BUSY)
            continue;

        APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
        if(  ptzControlCmdPos.ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_PAN)
            ptzQueryPan();
        else if(  ptzControlCmdPos.ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_TILT)
            ptzQueryTilt();
        else if(ptzControlCmdPos.ePtzControlCmd == EPtzControlCmd::PTZ_CONTROL_HOMIMG){

            if( !(fHomeCmdState & HomeCmdStatePanFinished)){
                ptzQueryPan();
            }else if( !(fHomeCmdState & HomeCmdStateTiltFinished)){
                ptzQueryTilt();
            }
        }
        
    }//end while
}

EPtzState ThermalControl::getState(){

    return currPtzState;
}

int ThermalControl::getPanAngle() {

    return currPtzPos.pan;
}

int ThermalControl::getTiltAngle() {

    return currPtzPos.tilt;
}

// int ThermalControl::getZoomAngle() {

//     return currPtzPos.zoom;
// }


int ThermalControl::getptzready(){
    
    return ptzready;

}

EPtzState ThermalControl::ptzHome(){
    if(currPtzState == EPtzState::PTZ_BUSY){

        return EPtzState::PTZ_ERROR_BUSY;
    }    

    HomeCoordinate_S homeCoordinate = ptzProtocolHandler.getHomeCoordinate();
    ptzControlCmdPos.posPan = (int)((0xFF&homeCoordinate.panH) <<8 | (0xFF&homeCoordinate.panL));
    ptzControlCmdPos.posTilt = (int)((0xFF&homeCoordinate.tiltH) <<8 | (0xFF&homeCoordinate.tiltL));

    bCallBackEnable = true;
    currPtzState = EPtzState::PTZ_BUSY;
    fHomeCmdState = 0x00;//reset, wait pan and tilt response
    ptzControlCmdPos.ePtzControlCmd = EPtzControlCmd::PTZ_CONTROL_HOMIMG;
    ptzProtocolHandler.ptzHome();
    return EPtzState::PTZ_BUSY;
}

void ThermalControl::ptzStop(){
    bCallBackEnable = false;
    ptzProtocolHandler.ptzStop();
    //update the latest positions of pan and tilt
    ptzQueryPan();
    usleep(200*1000);
    ptzQueryTilt();
    usleep(200*1000);
    currPtzState = EPtzState::PTZ_IDLE;
}
EPtzState ThermalControl::ptzLeft(){
    if(currPtzState == EPtzState::PTZ_BUSY){

        return EPtzState::PTZ_ERROR_BUSY;
    }
    bCallBackEnable = false;
    currPtzState = EPtzState::PTZ_BUSY;
    ptzProtocolHandler.ptzLeft();
    return EPtzState::PTZ_BUSY;
}
EPtzState ThermalControl::ptzRight(){
    if(currPtzState == EPtzState::PTZ_BUSY){

        return EPtzState::PTZ_ERROR_BUSY;
    }    
    bCallBackEnable = false;
    currPtzState = EPtzState::PTZ_BUSY;
    ptzProtocolHandler.ptzRight();
    return EPtzState::PTZ_BUSY;
}
EPtzState ThermalControl::ptzUp(){
    if(currPtzState == EPtzState::PTZ_BUSY){

        return EPtzState::PTZ_ERROR_BUSY;
    }    
    bCallBackEnable = false;
    currPtzState = EPtzState::PTZ_BUSY;
    ptzProtocolHandler.ptzUp();
    return EPtzState::PTZ_BUSY;    
}
EPtzState ThermalControl::ptzDown(){
    if(currPtzState == EPtzState::PTZ_BUSY){

        return EPtzState::PTZ_ERROR_BUSY;
    }    
    bCallBackEnable = false;
    currPtzState = EPtzState::PTZ_BUSY;
    ptzProtocolHandler.ptzDown();
    return EPtzState::PTZ_BUSY;    
}
void ThermalControl::ptzAutoFocus(){
    ptzProtocolHandler.ptzAutoFocus();
}
void ThermalControl::ptzZoomNear(){
    ptzProtocolHandler.ptzZoomNear();
}
void ThermalControl::ptzZoomFar(){
    ptzProtocolHandler.ptzZoomFar();
}
void ThermalControl::ptzQueryPan(){
    ptzProtocolHandler.ptzQueryPan();
}
void ThermalControl::ptzQueryTilt(){
    ptzProtocolHandler.ptzQueryTilt();
}

void ThermalControl::ptzQueryZoom(){
    ptzProtocolHandler.ptzQueryZoom();
}

void ThermalControl::ptzQueryFocus(){
    ptzProtocolHandler.ptzQueryFocus();
}

void ThermalControl::ptzSetPanTiltPosition(int pan, int tilt){
    ptzProtocolHandler.ptzSetPanTiltPosition(pan, tilt);
}

void ThermalControl::ptzSetZoomPosition(int zoom){
    ptzProtocolHandler.ptzSetZoomPosition(zoom);
}

/*Can't be interrupt by stop command*/
EPtzState ThermalControl::ptzSetPanPositionByAngle(int pan_angle){
    if(currPtzState == EPtzState::PTZ_BUSY){
 
        return EPtzState::PTZ_ERROR_BUSY;
    }

    APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
    if(pan_angle < -180 || pan_angle >180){

        return EPtzState::PTZ_ERROR_CMD_POS;
    }

    bCallBackEnable = true;
    int pan = 0;
    pan = HOME_PAN +10*pan_angle;  //HOME_PAN = 2353
    
    if(pan >= 3600) 
        pan -= 3600;

    ptzProtocolHandler.ptzSetPanTiltPosition(pan, currPtzPos.tilt);
    
    
    //currPtzPos.pan = pan;
    currPtzState = EPtzState::PTZ_BUSY;
    
    
    

    setPtzControlCmdPos(pan, EPtzControlCmd::PTZ_CONTROL_PAN);
    return EPtzState::PTZ_BUSY;
}

/*Can't be interrupt by stop command*/
EPtzState ThermalControl::ptzSetTiltPositionByAngle(int tilt_angle){
    if(currPtzState == EPtzState::PTZ_BUSY){

        return EPtzState::PTZ_ERROR_BUSY;
    }

    APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
    if(tilt_angle < -35 || tilt_angle > 40){
 
        return EPtzState::PTZ_ERROR_CMD_POS;
    }

    bCallBackEnable = true;
    int tilt = 0;
    tilt = HOME_TILT +10*tilt_angle*-1;//not test
    ptzProtocolHandler.ptzSetPanTiltPosition(currPtzPos.pan, tilt);
    //currPtzPos.tilt = tilt;
    currPtzState = EPtzState::PTZ_BUSY;

    setPtzControlCmdPos(tilt, EPtzControlCmd::PTZ_CONTROL_TILT);

    return EPtzState::PTZ_BUSY;
}
/*
//not tested
EPtzState ThermalControl::ptzShiftPanPositionByAngle(int pan_angle){//relative position
    if(currPtzState == EPtzState::PTZ_BUSY){
        printf("PTZ_BUSY, Cmd is Skip.\r\n");
        return EPtzState::PTZ_ERROR_BUSY;
    }

    APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
    if(pan_angle < -180 || pan_angle >180){
        printf("wrong cmd angle . shift angle = [%d]\r\n", pan_angle);
        return EPtzState::PTZ_ERROR_CMD_POS;
    }

    bCallBackEnable = true;
    int pan = 0;
    pan = currPtzPos.pan +10*pan_angle;

    ptzProtocolHandler.ptzSetPanTiltPosition(pan, currPtzPos.tilt);
    currPtzPos.pan = pan;
    setPtzControlCmdPos(pan, EPtzControlCmd::PTZ_CONTROL_PAN);
    currPtzState = EPtzState::PTZ_BUSY;
    return EPtzState::PTZ_BUSY;

}

//not tested
EPtzState ThermalControl::ptzShiftTiltPositionByAngle(int tilt_angle){
    if(currPtzState == EPtzState::PTZ_BUSY){
        printf("PTZ_BUSY, Cmd is Skip.\r\n");
        return EPtzState::PTZ_ERROR_BUSY;
    }

    APP_UTIL::AppMutex((APP_UTIL::AppCS*) cmdCS);
    if(tilt_angle < -35 || tilt_angle > 40){
        printf("wrong angle = [%d]\r\n", tilt_angle);
        return EPtzState::PTZ_ERROR_CMD_POS;
    }

    bCallBackEnable = true;
    int tilt = 0;
    tilt = currPtzPos.tilt +10*tilt_angle*-1;//not test

    ptzProtocolHandler.ptzSetPanTiltPosition(currPtzPos.pan, tilt);
    currPtzPos.tilt = tilt;
    setPtzControlCmdPos(tilt, EPtzControlCmd::PTZ_CONTROL_TILT);
    currPtzState = EPtzState::PTZ_BUSY;
    return EPtzState::PTZ_BUSY;
}
*/

void ThermalControl::dump(){

    printf("(pan=%d, tilt=%d)\r\n", currPtzPos.pan, currPtzPos.tilt);
    printf("(fHomeCmdState=%d, currPtzState=%d)\r\n", fHomeCmdState, static_cast<int>(currPtzState));
    
}
