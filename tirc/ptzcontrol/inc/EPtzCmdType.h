/* Fukuan Hsu 2021.04.19 */
#ifndef __PTZ_CMD_TYPE_H__
#define __PTZ_CMD_TYPE_H__
#include <stdio.h>

#define PTZ_CMD_NUM 20

enum class EPtzCmdType;
typedef struct _PtzCmdMapping
{
    unsigned char cmd;
    EPtzCmdType ptzCmdType;
}PtzCmdMapping;

enum class EPtzCmdType{

    PTZ_STOP = 0,
    PTZ_PAN_RIGHT = 1,
    PTZ_PAN_LEFT = 2,
    PTZ_TILT_UP = 3,
    PTZ_TILT_DOWN = 4,
    PTZ_FOCUS = 5,
    PTZ_ZOOM_NEAR = 6,
    PTZ_ZOOM_FAR = 7,
    PTZ_QUREY_PAN = 8,
    PTZ_QUERY_TILT = 9,
    PTZ_QUERY_ZOOM = 10,
    PTZ_QUERY_FOCUS = 11,
    PTZ_HOME = 12,
    PTZ_QUERY_PAN_RES=13,
    PTZ_QUERY_TILT_RES=14,
    PTZ_UNKNOWN =99
};


#endif //__PTZ_CMD_TYPE_H__