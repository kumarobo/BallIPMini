/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright � 2007-2008 Microchip Technology Inc. and its licensors.  All         
rights reserved.                                                                
                                                                                
Microchip licenses to you the right to: (1) install Software on a single        
computer and use the Software with Microchip 16-bit microcontrollers and        
16-bit digital signal controllers ("Microchip Product"); and (2) at your        
own discretion and risk, use, modify, copy and distribute the device            
driver files of the Software that are provided to you in Source Code;           
provided that such Device Drivers are only used with Microchip Products         
and that no open source or free software is incorporated into the Device        
Drivers without Microchip's prior written consent in each instance.             
                                                                                
You should refer to the license agreement accompanying this Software for        
additional information regarding your rights and obligations.                   
                                                                                
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY         
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY              
WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A          
PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE             
LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY,               
CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY           
DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY         
INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR         
LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY,                 
SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY         
DEFENSE THEREOF), OR OTHER SIMILAR COSTS.                                       
                                                                                
********************************************************************************
*/

// Created by the Microchip USBConfig Utility, Version 1.0.4.0, 4/25/2008, 17:05:22

#ifndef _usb_config_h_
#define _usb_config_h_

// Supported USB Configurations


#define USB_SUPPORT_HOST

// Hardware Configuration

#define USB_PING_PONG_MODE  USB_PING_PONG__FULL_PING_PONG

// Host Configuration

#define NUM_TPL_ENTRIES 2
#define USB_NUM_CONTROL_NAKS 20
#define USB_SUPPORT_INTERRUPT_TRANSFERS
#define USB_NUM_INTERRUPT_NAKS 20
#define USB_INITIAL_VBUS_CURRENT (100/2)
#define USB_INSERT_TIME (250+1)
#define USB_HOST_APP_EVENT_HANDLER USB_ApplicationEventHandler
#define USB_ENABLE_TRANSFER_EVENT

// Host HID Client Driver Configuration

#define USB_MAX_HID_DEVICES 1
#define HID_MAX_DATA_FIELD_SIZE 8
#define APPL_COLLECT_PARSED_DATA USB_HID_DataCollectionHandler

// Helpful Macros

#define USBTasks()                  \
    {                               \
        USBHostTasks();             \
        USBHostHIDTasks();          \
    }

#define USBInitialize(x)            \
    {                               \
        USBHostInit(x);             \
    }


#endif

