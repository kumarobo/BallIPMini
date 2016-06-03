/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

/* Modified for Joypad by Kumagai. M */

#include "usb_host_hid.h"

/*********************************************************************
* Function: void APP_HostHIDKeyboardInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostHIDJoyPadInitialize();

/*********************************************************************
* Function: void APP_HostHIDKeyboardTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized via
*   the APP_HostHIDKeyboardInitialize() function
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostHIDJoyPadTasks();

/*********************************************************************
* Function: bool APP_HostHIDKeyboardReportParser(void);
*
* Overview: Parses the report descriptor to determine if the report
*           matches what this demo will support
*
* PreCondition: None
*
* Input: None
*
* Output: bool - true if the demo supports this device, false otherwise.
*
********************************************************************/
bool APP_HostHIDJoyPadReportParser(void);


/* added by Kumagai */
/* Non-timer kick of HID Input reuqest(vs. TIMER_RequestTick(&APP_HostHIDTimerHandler, 10);*/
extern void APP_HostHIDInputRequest(void);

typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned long int dword;

/* JoyPad definition table */
typedef struct _axisconf
{
   byte ofs;
   bool signrev;
} AxisConf;

typedef struct _PadConfig
{
   word vid,pid;
   AxisConf axisconf[4];
   byte buttonofsu,buttonofsd;
   word buttonremask[16];
} PadConfig;

typedef struct _PadState
{
    word axis[4],lastaxis[4];
    // while pads have 8bit in most cast, some have 10 or more bits
    // therefore, 16bits for each axis with padding 0 to lower.
    word button,lastbutton;   // button mask
    word inputcount;  // input report count, incremented periodically if valid
    bool inuse,updated;    // active or not
} PadState;
extern PadState padState;

//      / \ Axis1 +            / \ Axis3 +               [B3]
//       |                      |                    [B0]    [B1] 
//   ----+---->  Axis0 +    ----+---->  Axis2 +          [B2]
//       |                      |
//  L_upper [B4]    R_upper [B6]
//  L_upper [B5]    R_upper [B7]
//  Select [B8]  Select2[B9]
//  Additional [B10]-[B15]
//  Note: axis modifed 2016/1/7 (exchange, 0-1, 2-3)
