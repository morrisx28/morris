#ifndef __THERMAL_DEMO_H__
#define __THERMAL_DEMO_H__
#include <string.h>
#include "PtzCmdEventInf.h"
#include "ThermalControl.h"
using namespace std;
class ThermalDemo: public PtzCmdEventInf {

    ThermalControl *thermalControl;
    void doCmd(string str);
public:
    ThermalDemo();
    ~ThermalDemo();
    int open();
    int close();
    void runLoop();
    void received(EPtzControlCmd type, int status);
    void runCmdThread();
    static void *_ThreadCmd(void *arg){
        ((ThermalDemo *)arg)->runCmdThread();
        return ((void*)0);
    }
};

#endif //__THERMAL_DEMO_H__