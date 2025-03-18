/*
 * HTTP_lib.h
 * 
 *  Author: Tran Doan Manh
*/

#ifndef _HTTP_LIB_H
#define _HTTP_LIB_H

    /*------------------------------- All libraries -------------------------------*/



    /*------------------------------- Declare enum type & function prototypes -------------------------------*/

    // Enum type
typedef enum { // Define the Web interface
    HTTP_LoginInterface,
    HTTP_ControlInterface
}HTTP_webInterface_e;

    // Function prototypes
void HTTP_startStairSwitchServer(void);
void HTTP_stopStairSwitchServer(void);
                    
#endif

    /*------------------------------- END THIS FILE -------------------------------*/