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
#include "usb.h"
#include "usb_host_hid.h"

#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ===============================
// compatible functions by kumagai
#include "brc.h"
#include "slnode.h"
#include "app_host_hid_joypad.h"
void PRINT_ClearScreen(void) {}
void PRINT_CursorEnable(bool b) {}
void LED_On(int s) { /* BRC_LEDGreen(1); */ }
void LED_Off(int s) { /* BRC_LEDGreen(0); */ }
#define LED_USB_HOST_HID_JOYPAD_DEVICE_READY 0

void PRINT_String(char *str,int len)
{
    SLReplyProfile(63,str);
}

const char *_hexstr="0123456789ABCDEF";
void PRINT_StringWithNum(char *str,unsigned int n)
{
    int i,t;
    char tmp[80];
    for(i=0;(i<60)&&(*str);i++,str++)
        tmp[i]=*str;
    t=n;
    tmp[i+4]=_hexstr[t%10]; t=t/10;
    tmp[i+3]=(t==0)?' ':_hexstr[t%10]; t=t/10;
    tmp[i+2]=(t==0)?' ':_hexstr[t%10]; t=t/10;
    tmp[i+1]=(t==0)?' ':_hexstr[t%10]; t=t/10;
    tmp[i+0]=(t==0)?' ':_hexstr[t%10];
    tmp[i+5]=' '; i+=6;
    t=n;
    tmp[i+3]=_hexstr[t&0xf]; t=t>>4;
    tmp[i+2]=_hexstr[t&0xf]; t=t>>4;
    tmp[i+1]=_hexstr[t&0xf]; t=t>>4;
    tmp[i+0]=_hexstr[t&0xf]; t=t>>4;
    tmp[i+4]='\0';
    PRINT_String(tmp,i+4);
}

void PRINT_Char(char b)
{
    char work[8];
    work[0]='K';
    work[1]=':';
    work[2]='[';
    work[3]=((b>=0x20)&&(b<0x7f))?b:'.';
    work[4]=']';
    work[5]='\0';
    PRINT_String(work,5);
}

// 1ms step timer request; simple hook
typedef void (*TICK_HANDLER)(void);
static inline bool TIMER_RequestTick(TICK_HANDLER handle, uint32_t rate)
{return true;} // dummy


// USB Interrupt Service

void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
        USB_HostInterruptHandler();
}


// *****************************************************************************
// *****************************************************************************
// Type definitions
// *****************************************************************************
// *****************************************************************************

typedef enum _APP_STATE
{
    DEVICE_NOT_CONNECTED,
    WAITING_FOR_DEVICE,
    DEVICE_CONNECTED, /* Device Enumerated  - Report Descriptor Parsed */
    GET_INPUT_REPORT, /* perform operation on received report */
    INPUT_REPORT_PENDING,
    SEND_OUTPUT_REPORT, /* Not needed in case of mouse */
    OUTPUT_REPORT_PENDING,
    ERROR_REPORTED
} JOYPAD_STATE;


typedef struct
{
    uint8_t address;
    char padtype;
    JOYPAD_STATE state;
    bool inUse;

    struct 
    {
        uint16_t id;
        uint16_t size;
        uint16_t pollRate;
        uint8_t *buffer;
        struct
        {
            HID_DATA_DETAILS details;
            uint8_t data[20];
//                HID_USER_DATA_SIZE newData[6];
//                HID_USER_DATA_SIZE oldData[6];
        } parsed;
    } pad;
        
    /*
    struct
    {
        uint16_t id;
        uint16_t size;
        uint16_t pollRate;
        uint8_t *buffer;

        struct
        {           
            struct
            {
                HID_DATA_DETAILS details;
                HID_USER_DATA_SIZE newData[6];
                HID_USER_DATA_SIZE oldData[6];
            } parsed;
        } normal;

        struct
        {
            struct{
                HID_USER_DATA_SIZE data[8];
                HID_DATA_DETAILS details;
            } parsed;
        } modifier;
    } keys;

    struct
    {
        bool updated;
        
        union
        {
            uint8_t value;
            struct
            {
                uint8_t  numLock       : 1;
                uint8_t  capsLock      : 1;
                uint8_t  scrollLock    : 1;
                uint8_t                : 5;
            } bits;
        } report;
        struct
        {
            HID_DATA_DETAILS details;
        } parsed;
    } leds;*/
} JOYPAD;


// pad definition table
const PadConfig padConfig[] =
{
	{  // elecom wired pad
   		0xF766,0x0001,
   		{ { 2, false}, { 3,true },  {1,false}, {0, true}  },
   		5,4,
  		{ 0x0010,0x0080,0x0040,0x0020, 0x0100,0x0400,0x0200,0x0800,
     	  0x4000,0x8000,0x1000,0x2000, 0x0000,0x0000,0x0000,0x0000 }   
 	},
	{   // logicool wheel
   		0x046D,0xC294,
   		{ { 1, false}, { 3,true },  {1,false}, {3, true} },
   		1,2,
  		{ 0x0800,0x1000,0x0400,0x2000, 0x0002,0x8000,0x0001,0x4000,
     	  0x0004,0x0008,0x0020,0x0010, 0x0000,0x0000,0x0000,0x0000 }   
 	},
	{   // elecom wireless pad
   		0x05B8,0x1006,
   		{ { 3, false}, { 4,true },  {5,false}, {6, true} },
   		0,1,
  		{ 0x0100,0x0800,0x0400,0x0200, 0x1000,0x4000,0x2000,0x8000,
     	  0x0001,0x0002,0x0004,0x0008, 0x0000,0x0000,0x0000,0x0000 }   
 	},
    { 0,0, }
};

PadState padState; // pad input state



#define MAX_ERROR_COUNTER               (10)

// *****************************************************************************
// *****************************************************************************
// Local Variables
// *****************************************************************************
// *****************************************************************************

static JOYPAD joypad;


// *****************************************************************************
// *****************************************************************************
// Local Function Prototypes
// *****************************************************************************
// *****************************************************************************
static void App_ProcessInputReport(void);

// *****************************************************************************
// *****************************************************************************
// Functions
// *****************************************************************************
// *****************************************************************************

/*********************************************************************
* Function: void APP_HostHIDTimerHandler(void);
*
* Overview: Switches over the state machine state to get a new report
*           periodically if the device is idle
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
static void APP_HostHIDTimerHandler(void)
{
    if(joypad.state == DEVICE_CONNECTED)
    {
        joypad.state = GET_INPUT_REPORT;
    }
}

void APP_HostHIDInputRequest(void)
{
    APP_HostHIDTimerHandler();
}

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
void APP_HostHIDJoyPadInitialize()
{
    joypad.state = DEVICE_NOT_CONNECTED;
    joypad.inUse = false;
    joypad.pad.buffer = NULL;
    joypad.address = 0;
    padState.axis[0]=padState.axis[1]=padState.axis[2]=padState.axis[3]=0;
    padState.button=0;
    padState.inuse=false;
    padState.updated=false;
    padState.inputcount=0;
}

/*********************************************************************
* Function: void APP_HostHIDKeyboardTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized via
*   the APP_HostHIDKeyboardInitialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostHIDJoyPadTasks()
{
    uint8_t error;
    uint8_t count;
    
    if(joypad.address == 0)
    {
        joypad.address = USBHostHIDDeviceDetect();
    }
    else
    {
        if(USBHostHIDDeviceStatus(joypad.address) == USB_HID_DEVICE_NOT_FOUND)
        {
            joypad.state = DEVICE_NOT_CONNECTED;
            joypad.address = 0;
            joypad.inUse = false;
            padState.inuse=false;
            padState.updated=false;
            
            if(joypad.pad.buffer != NULL)
            {
                free(joypad.pad.buffer);
                joypad.pad.buffer = NULL;
            }
        }      
    }

    switch(joypad.state)
    {
        case DEVICE_NOT_CONNECTED:
            //PRINT_ClearScreen();
            PRINT_String("Attach joypad\r\n", 17);
            joypad.state = WAITING_FOR_DEVICE;
            LED_Off(LED_USB_HOST_HID_JOYPAD_DEVICE_READY);
            PRINT_CursorEnable(false);
            break;
            
        case WAITING_FOR_DEVICE:
            if( (joypad.address != 0) &&
                (USBHostHIDDeviceStatus(joypad.address) == USB_HID_NORMAL_RUNNING)
              ) /* True if report descriptor is parsed with no error */
            {
                PRINT_ClearScreen();
                PRINT_CursorEnable(true);
                LED_On(LED_USB_HOST_HID_JOYPAD_DEVICE_READY);
                /*Kuma*/PRINT_String("padcon", 17);
                
                joypad.state = DEVICE_CONNECTED;
                TIMER_RequestTick(&APP_HostHIDTimerHandler, 10);
            }
            break;
            
        case DEVICE_CONNECTED:
            break;

        case GET_INPUT_REPORT:
            if(USBHostHIDRead(
                                joypad.address,
                                joypad.pad.id,
                                joypad.pad.parsed.details.interfaceNum,
                                joypad.pad.size,
                                joypad.pad.buffer
                             )
              )
            {
                /* Host may be busy/error -- keep trying */
            }
            else
            {
                joypad.state = INPUT_REPORT_PENDING;
            }
            break;

        case INPUT_REPORT_PENDING:
            if(USBHostHIDReadIsComplete(joypad.address, &error, &count))
            {
                if(error || (count == 0))
                {
                    joypad.state = DEVICE_CONNECTED;
                }
                else
                {
                    joypad.state = DEVICE_CONNECTED;

                    App_ProcessInputReport();
                }
            }
            break;

#if 0
        case SEND_OUTPUT_REPORT: /* Will be done while implementing Keyboard */
/*            if(USBHostHIDWrite(    
                                keyboard.address,
                                keyboard.leds.parsed.details.reportID,
                                keyboard.leds.parsed.details.interfaceNum,
                                keyboard.leds.parsed.details.reportLength,
                                (uint8_t*)&keyboard.leds.report
                               )
              )
            {
                * Host may be busy/error -- keep trying *
            }
            else
            {
                keyboard.state = OUTPUT_REPORT_PENDING;
            }*/
            joypad.state = DEVICE_CONNECTED;
            break;

        case OUTPUT_REPORT_PENDING:
/*            if(USBHostHIDWriteIsComplete(keyboard.address, &error, &count))
            {
                keyboard.leds.updated = false;
                keyboard.state = DEVICE_CONNECTED;
            }*/
            joypad.state = DEVICE_CONNECTED;
            break;
#endif

        case ERROR_REPORTED:
            Nop();
            Nop();
            Nop();
            break;

        default:
            Nop();
            Nop();
            Nop();
            break;

    }
}

/****************************************************************************
  Function:
    BOOL USB_HID_DataCollectionHandler(void)
  Description:
    This function is invoked by HID client , purpose is to collect the
    details extracted from the report descriptor. HID client will store
    information extracted from the report descriptor in data structures.
    Application needs to create object for each report type it needs to
    extract.
    For ex: HID_DATA_DETAILS keyboard.keys.modifier.details;
    HID_DATA_DETAILS is defined in file usb_host_hid_appl_interface.h
    Each member of the structure must be initialized inside this function.
    Application interface layer provides functions :
    USBHostHID_ApiFindBit()
    USBHostHID_ApiFindValue()
    These functions can be used to fill in the details as shown in the demo
    code.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    true    - If the report details are collected successfully.
    false   - If the application does not find the the supported format.

  Remarks:
    This Function name should be entered in the USB configuration tool
    in the field "Parsed Data Collection handler".
    If the application does not define this function , then HID cient
    assumes that Application is aware of report format of the attached
    device.
***************************************************************************/
bool APP_HostHIDJoyPadReportParser(void)
{
    uint8_t NumOfReportItem = 0;
    uint8_t i;
    USB_HID_ITEM_LIST* pitemListPtrs;
    USB_HID_DEVICE_RPT_INFO* pDeviceRptinfo;
    HID_REPORTITEM *reportItem;
    HID_USAGEITEM *hidUsageItem;
    uint8_t usageIndex;
    uint8_t reportIndex;
    bool foundPad = false;

    /* The keyboard is already in use. */
    if(joypad.inUse == true)
    {
        return false;
    }

    pDeviceRptinfo = USBHostHID_GetCurrentReportInfo(); // Get current Report Info pointer
    pitemListPtrs = USBHostHID_GetItemListPointers();   // Get pointer to list of item pointers

    
    /* Find Report Item Index for Modifier Keys */
    /* Once report Item is located , extract information from data structures provided by the parser */
    NumOfReportItem = pDeviceRptinfo->reportItems;
    PRINT_StringWithNum("NumOfReportItem:",NumOfReportItem);
    for(i=0;i<NumOfReportItem;i++)
    {
        reportItem = &pitemListPtrs->reportItemList[i];
        //PRINT_StringWithNum("reportType:",reportItem->reportType);
        //PRINT_StringWithNum("dataModes:",reportItem->dataModes);
        //if((reportItem->reportType==hidReportInput)&& (reportItem->dataModes == HIDData_Array))
        //if(reportItem->globals.usagePage==USB_HID_USAGE_PAGE_GAME_CONTROLS)
        //PRINT_StringWithNum("dataModes:",reportItem->dataModes);
        //PRINT_StringWithNum("usagePage:",reportItem->globals.usagePage);
        
        //PRINT_StringWithNum("reportType:",reportItem->reportType);
        //PRINT_StringWithNum("dataModes:",reportItem->dataModes);
        //PRINT_StringWithNum("usagePage:",reportItem->globals.usagePage);
        //long _k;
        //for(_k=0;_k<500000l;_k++) ;
        if((reportItem->reportType==hidReportInput) && (reportItem->dataModes == HIDData_Variable)&&
           (reportItem->globals.usagePage==USB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS))
        {
            PRINT_String("found GENERIC_DESKTOP_CONTROLS",0);
            /* We now know report item points to modifier keys */
            /* Now make sure usage Min & Max are as per application */
            usageIndex = reportItem->firstUsageItem;
            hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];
            reportIndex = reportItem->globals.reportIndex;
            joypad.pad.parsed.details.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
            joypad.pad.parsed.details.reportID = (uint8_t)reportItem->globals.reportID;
            joypad.pad.parsed.details.bitOffset = (uint8_t)reportItem->startBit;
            joypad.pad.parsed.details.bitLength = (uint8_t)reportItem->globals.reportsize;
            joypad.pad.parsed.details.count = (uint8_t)reportItem->globals.reportCount;
            joypad.pad.parsed.details.interfaceNum = USBHostHID_ApiGetCurrentInterfaceNum();
            foundPad = true;
            PRINT_StringWithNum("index:",reportIndex);
            PRINT_StringWithNum("len:",joypad.pad.parsed.details.reportLength);
            PRINT_StringWithNum("bitlen:",joypad.pad.parsed.details.bitLength);
        }
    }

    if(pDeviceRptinfo->reports > 0)
    {
        joypad.pad.id = 0;
        joypad.pad.size = joypad.pad.parsed.details.reportLength;
        joypad.pad.buffer = (uint8_t*)malloc(joypad.pad.size);
        joypad.pad.pollRate = pDeviceRptinfo->reportPollingRate;

        //if(foundPad == true)
        //    joypad.inUse=true;

        USB_DEVICE_DESCRIPTOR *pDesc=(USB_DEVICE_DESCRIPTOR *)USBHostGetDeviceDescriptor(0);
        unsigned int vid=pDesc->idVendor;
        unsigned int pid=pDesc->idProduct;
        PRINT_StringWithNum("VID:",vid);
        PRINT_StringWithNum("PID:",pid);
        
        joypad.padtype=-1;
        for(i=0;padConfig[i].vid!=0;i++)
        {
            if((vid==padConfig[i].vid)&&(pid==padConfig[i].pid))
            {
                joypad.padtype=i;
                PRINT_StringWithNum("Found PadConfig No:",i);
                joypad.inUse=true;
            }
        }
    }
    padState.inuse=joypad.inUse;
    return(joypad.inUse);
}

/****************************************************************************
  Function:
    void App_ProcessInputReport(void)

  Description:
    This function processes input report received from HID device.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
static void App_ProcessInputReport(void)
{
    int i;
    uint16_t ax;
    bool updated=false;
    //SLNop();
    USBHostHID_ApiImportData(   joypad.pad.buffer,
                                joypad.pad.size,
                                joypad.pad.parsed.data,
                                &joypad.pad.parsed.details
                            );
#if 0
    {
        char tmp[60],*s;
        PRINT_String("HID rawdata",0);
        i=0;
        while(i<joypad.pad.size)
        {
            for(j=0,s=tmp;(j<16)&&(i<joypad.pad.size);i++,j++)
            {
                *s=_hexstr[(joypad.pad.buffer[i]>>4)&0xf]; s++;
                *s=_hexstr[(joypad.pad.buffer[i]   )&0xf]; s++;
                *s=' '; s++; if(j==7) { *s=' '; s++; }
            }
            *s='\0';
            PRINT_String(tmp,0);
        }
    }
#endif
    if(joypad.padtype>=0)
	{
        const PadConfig *pc=padConfig+joypad.padtype;
        uint8_t *p=joypad.pad.buffer;
		for(i=0;i<4;i++)
		{
            padState.lastaxis[i]=padState.axis[i];
            ax=p[pc->axisconf[i].ofs];
            if(pc->axisconf[i].signrev) ax=~(ax); // 255-ax[i];
            padState.axis[i]=ax<<8;  // 8bit axis
            if(padState.lastaxis[i]!=padState.axis[i]) updated=true;
		}
		// exceptions
		if(joypad.padtype==1) //  wheel; 10bit
		{
            padState.axis[1]=padState.axis[3]=
              (((uint16_t)(p[1]&0x03))<<14)| (((uint16_t)(p[0]))<<6);
        }
        uint16_t btpad,btremap,m;
		btpad=(((uint16_t)(p[pc->buttonofsu]))<<8)|(p[pc->buttonofsd]);
		btremap=0;
		for(i=0,m=1;i<16;i++,m=m<<1)
		{
			if(btpad&(pc->buttonremask[i])) 
				btremap|=m;
		}
        padState.lastbutton=padState.button;
        padState.button=btremap;
        if(padState.lastbutton!=padState.button) updated=true;
    }
    padState.updated=updated;
    padState.inputcount++;
	return ;
}

/****************************************************************************
  Function:
    bool USB_ApplicationEventHandler( uint8_t address, USB_EVENT event,
                void *data, uint32_t size )

  Summary:
    This is the application event handler.  It is called when the stack has
    an event that needs to be handled by the application layer rather than
    by the client driver.

  Description:
    This is the application event handler.  It is called when the stack has
    an event that needs to be handled by the application layer rather than
    by the client driver.  If the application is able to handle the event, it
    returns true.  Otherwise, it returns false.

  Precondition:
    None

  Parameters:
    uint8_t address    - Address of device where event occurred
    USB_EVENT event - Identifies the event that occured
    void *data      - Pointer to event-specific data
    uint32_t size      - Size of the event-specific data

  Return Values:
    true    - The event was handled
    false   - The event was not handled

  Remarks:
    The application may also implement an event handling routine if it
    requires knowledge of events.  To do so, it must implement a routine that
    matches this function signature and define the USB_HOST_APP_EVENT_HANDLER
    macro as the name of that function.
  ***************************************************************************/
bool USB_HOST_APP_EVENT_HANDLER ( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
    switch( (int)event )
    {
        /* Standard USB host events ******************************************/
        case EVENT_VBUS_REQUEST_POWER:
        case EVENT_VBUS_RELEASE_POWER:
        case EVENT_HUB_ATTACH:
        case EVENT_UNSUPPORTED_DEVICE:
        case EVENT_CANNOT_ENUMERATE:
        case EVENT_CLIENT_INIT_ERROR:
        case EVENT_OUT_OF_MEMORY:
        case EVENT_UNSPECIFIED_ERROR:
            return true;
            break;

        /* HID Class Specific Events ******************************************/
        case EVENT_HID_RPT_DESC_PARSED:
            if(APP_HostHIDJoyPadReportParser() == true)
            {
                return true;
            }
            break;

        default:
            break;
    }

    return false;

}

