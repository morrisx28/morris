#ifndef __THERMAL_CONTROL_H__
#define __THERMAL_CONTROL_H__
#include <stdio.h>
#include <stdlib.h>

#include "PtzProtocolHandler.h"
#include "EPtzCmdType.h"
#include "EPtzState.h"
#include "EPtzControlCmd.h"
#include "PtzCmdEventInf.h"

/*Unit =>  Pan: 0(left) to 3599(right); Tilt: -400(up) to 350 (down)*/
typedef struct _PTZ_POS_S{

    int tilt;
    int pan;
    int zoom;
    int focus;

}PTZ_POS_S;

typedef struct _PTZ_CMD_POS_S{

    int posPan;
    int posTilt;
    EPtzControlCmd ePtzControlCmd;

}PTZ_CMD_POS_S;


class ThermalControl: public ProtocolHandleInf{
private:
    /*variable*/
    PtzProtocolHandler ptzProtocolHandler;
    PTZ_POS_S currPtzPos;//the position of current camera.
    PTZ_POS_S tempPtzPos;//used to check if the camera is ready
    int ptzready;
    EPtzState currPtzState;
    AppCS *cmdCS;
    pthread_t ptzCmdThread;
    PTZ_CMD_POS_S ptzControlCmdPos;
    bool bCallBackEnable = false;
    unsigned char fHomeCmdState = 0x00;//flag of recording the state of HomeCmd
    void setPtzControlCmdPos(int pos, EPtzControlCmd ePtzControlCmd);
    void ptzSetPanTiltPosition(int pan, int tilt);//private 
    PtzCmdEventInf *cbHander;
public:
    ThermalControl(/* args */);
    ~ThermalControl();
    int init();
    int open();
    int close();
    void setPtzCmdEventHandler(PtzCmdEventInf *handler);
    int received(EPtzCmdType type, char *data, int size);
    void runCmdThread();
    static void *_ThreadCmd(void *arg){
        ((ThermalControl *)arg)->runCmdThread();
        return ((void*)0);
    }
    EPtzState getState();
    int getPanAngle();
    int getTiltAngle();
    // int getZoomAngle();
    int getptzready();
    void reset();
    EPtzState ptzHome();
    void ptzStop();
    EPtzState ptzLeft();
    EPtzState ptzRight();
    EPtzState ptzUp();
    EPtzState ptzDown();
    void ptzAutoFocus();
    void ptzZoomNear();
    void ptzZoomFar();
    void ptzQueryPan();
    void ptzQueryTilt();
    void ptzQueryZoom();
    void ptzQueryFocus();

    void ptzSetZoomPosition(int zoom);
    EPtzState ptzSetPanPositionByAngle(int pan_angle);//absolute position
    EPtzState ptzSetTiltPositionByAngle(int tilt_angle);
    EPtzState ptzShiftPanPositionByAngle(int pan_angle);//relative position
    EPtzState ptzShiftTiltPositionByAngle(int tilt_angle);
    void dump();//only for debug
};

#endif//__THERMAL_CONTROL_H__