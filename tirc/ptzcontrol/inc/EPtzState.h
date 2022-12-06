/* Fukuan Hsu 2021.04.19 */
#ifndef __PTZ_STATE_H__
#define __PTZ_STATE_H__
#include <stdio.h>


enum class EPtzState{

    PTZ_IDLE = 0,
    PTZ_BUSY = 1,
    PTZ_ERROR_BUSY = 2,
    PTZ_ERROR_CMD_POS = 3,
    PTZ_ERROR_NO_RES = 4,
    PTZ_UNKNOWN =99
};


#endif //__PTZ_STATE_H__