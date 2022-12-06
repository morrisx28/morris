#ifndef __PTZ_CMD_EVENT_INF_H__
#define __PTZ_CMD_EVENT_INF_H__
#include "EPtzControlCmd.h"
class PtzCmdEventInf {

public:
    
    virtual ~PtzCmdEventInf(){}
    virtual void received(EPtzControlCmd type, int status)=0;
};

#endif //__PROTOCOL_HANDLE_INF_H__