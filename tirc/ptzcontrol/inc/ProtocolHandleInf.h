#ifndef __PROTOCOL_HANDLE_INF_H__
#define __PROTOCOL_HANDLE_INF_H__
#include "EPtzCmdType.h"
class ProtocolHandleInf {

public:
    
    virtual ~ProtocolHandleInf(){}
    virtual int received(EPtzCmdType type, char *data, int size)=0;
};

#endif //__PROTOCOL_HANDLE_INF_H__