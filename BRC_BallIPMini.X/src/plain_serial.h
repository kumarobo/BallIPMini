// Plain Serial Interface for dsPIC using UART2
// by Kumagai, M.

#include <p33Exxxx.h>
#include "uart.h"

// transmit buffer size (as ring buffer)
#ifndef SITXBLEN
#define SITXBLEN 256
#endif

// transmit buffer size (as ring buffer)
#ifndef SIRXBLEN
#define SIRXBLEN 256
#endif

#define SIB115200 1152
#define SIB57600   576


typedef struct _SIWork
{
	// transmit
	unsigned char txb[SITXBLEN];
	volatile int twp,trp; /* ring counter */
    // wp; write pointer; index at where next data will be writtern
    //                    incremented just after adding new char
    // rp; read pointer; index at where next data will be read
    //                    incremented just after reading a char

	// receive
	unsigned char rxb[SIRXBLEN];
	volatile int rwp,rrp; /* ring counter */

} SIWork;

extern void InitializeSerial(int baudrate);
extern int SerialWrite(unsigned char *data,int size);
extern int SerialWritePartial(unsigned char *data,int size);
extern int SerialRead(unsigned char *data,int size);
extern int SerialReadPartial(unsigned char *data,int size);
extern int SerialGetTxCount(void);
extern int SerialGetRxCount(void);
extern int SerialGetTxSpace(void);
extern int SerialGetRxSpace(void);
extern void SerialClearTxBuffer(void);
extern void SerialClearRxBuffer(void);

