#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "inc/ThermalDemo.h"

#define HOME_PAN 1800
#define HOME_TILT 0

using namespace std;
ThermalDemo::ThermalDemo(){

}

ThermalDemo::~ThermalDemo(){

}

void ThermalDemo::runCmdThread(){

    char str[100]={0};
    while(1){
        printf("Please input a string.\r\n");
        memset((void*)str, 0, sizeof(str));
        char *ret_val = fgets(str, sizeof(str), stdin);
        if(ret_val){

            char *find = strchr(str, '\n');
            if(find)
                *find = '\0';
        }
        string newStr(str);
        printf("find  [%s]\r\n",  str);
        doCmd(newStr);
    }

}

void ThermalDemo::doCmd(string str){

    if(str.compare("stop") == 0)
        thermalControl->ptzStop();
    else if(str.compare("up") == 0)
        thermalControl->ptzUp();
    else if(str.compare("down") == 0)
        thermalControl->ptzDown();
    else if(str.compare("left") == 0)
        thermalControl->ptzLeft();
    else if(str.compare("right") == 0)
        thermalControl->ptzRight();                        
    else if(str.compare("pan") == 0)
        thermalControl->ptzQueryPan();
    else if(str.compare("tilt") == 0)
        thermalControl->ptzQueryTilt();                    
    else if(str.compare("focus") == 0)
        thermalControl->ptzAutoFocus(); 
    else if(str.compare("queryfocus") == 0)
        thermalControl->ptzQueryFocus();
    else if(str.compare("queryzoom") == 0)
        thermalControl->ptzQueryZoom();      
    else if(str.compare("setzoom") == 0)
        thermalControl->ptzSetZoomPosition(0);                       
    else if(str.compare("home") == 0)
        thermalControl->ptzHome(); 
    else if(str.compare("home30") == 0)
        thermalControl->ptzSetPanPositionByAngle(30); 
    else if(str.compare("home90") == 0)
        thermalControl->ptzSetPanPositionByAngle(90);         
    else if(str.compare("home130") == 0)
        thermalControl->ptzSetPanPositionByAngle(130);         
    else if(str.compare("home179") == 0)
        thermalControl->ptzSetPanPositionByAngle(179);                      
    else if(str.compare("home180") == 0)
        thermalControl->ptzSetPanPositionByAngle(180); 
    else if(str.compare("home_180") == 0)
        thermalControl->ptzSetPanPositionByAngle(-180);         
    else if(str.compare("home0") == 0)
        thermalControl->ptzSetPanPositionByAngle(0);         
    else if(str.compare("tilt40") == 0)
        thermalControl->ptzSetTiltPositionByAngle(40); 
    else if(str.compare("tilt0") == 0)
        thermalControl->ptzSetTiltPositionByAngle(0);     
    else if(str.compare("tilt_35") == 0)
        thermalControl->ptzSetTiltPositionByAngle(-35);
    else if(str.compare("1") == 0)
        thermalControl->ptzHome();                          
    else if(str.compare("dump") == 0)
        thermalControl->dump(); 
}



int ThermalDemo::open(){
    int result = 0;
    thermalControl = new ThermalControl();
    thermalControl->setPtzCmdEventHandler(this);
    result = thermalControl->init();
    if(result == -1 ) 
        return -1;
    result = thermalControl->open();
    if(result == -1 ) 
        return -1;

    pthread_t keyThread;
    int error = pthread_create(&keyThread, NULL, _ThreadCmd, (void*)this);
    if(error != 0){
        printf("Can't create _ThreadReceiver!\r\n");
        return -1;
    }
    return 1;
}

int ThermalDemo::close(){
    int result = thermalControl->close();
    /* hanldel*/
    return 1;
}
void ThermalDemo::runLoop(){
    while(1)
        usleep(1000*10);
}
void ThermalDemo::received(EPtzControlCmd type, int status){

    printf("@@@ Got Event cmd = [%d], status = [%d]\r\n", static_cast<int>(type), status);
}
