/* Fukuan Hsu 2021.04.19 */
#ifndef __PTZ_CONTROL_CMD_H__
#define __PTZ_CONTROL_CMD_H__
#include <stdio.h>

enum class EPtzControlCmd{
    PTZ_CONTROL_IDLE =0,
    PTZ_CONTROL_PAN = 1,
    PTZ_CONTROL_TILT = 2,
    PTZ_CONTROL_HOMIMG = 3,
};


#endif //__PTZ_CONTROL_CMD_H__