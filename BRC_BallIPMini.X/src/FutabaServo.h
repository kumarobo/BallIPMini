// Futaba Serial Servo Interface for dsPIC using UART2
// based on Plain Serial
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
    
    // Sequence
    int fsc_txlen;  // if non-zero, simply transmit buffer, and dec
    unsigned char *fsrxw_len;  // non-NULL : rest count of waiting bytes
    unsigned char *fsrxw_to;   // non-NULL : timeout
} SIWork;

#include "BRC_PinDef.csv"
#define FS_RPIN RPIN_CCN36
#define FS_RPOUT RPOUT_CCN35
#define FS_TRISOE TRIS_CCN37
#define FS_LATOE LAT_CCN37

#define FS_485Enable()  FS_LATOE=1
#define FS_485Disable() FS_LATOE=0

enum FSCommands
{
    FSCTX=11,
    FSCRXCLEAR=12,
    FSCRXWAIT=13,
};

extern void InitializeSerial(int baudrate);
extern int SerialWrite(unsigned char *data,int size);
extern int SerialWritePartial(unsigned char *data,int size);
extern int SerialRead(unsigned char *data,int size);
extern int SerialReadPartial(unsigned char *data,int size);
extern int SerialReadPartialNonDestructive(unsigned char *data,int size);
extern int SerialGetTxCount(void);
extern int SerialGetRxCount(void);
extern int SerialGetTxSpace(void);
extern int SerialGetRxSpace(void);
extern void SerialClearTxBuffer(void);
extern void SerialClearRxBuffer(void);

// Servo Commands
extern void InitializeFutabaServo(int baudrate);
extern int FSSendShortCommand(int id,int addr,int count,unsigned char *data);
extern int FSSendLongCommand(int addr,int count,int byteseach,unsigned char *data);
extern int FSSendLongCommandByteWithChannel(int addr,int count,unsigned char *ch,unsigned char *vals);
extern int FSSendLongCommandWordWithChannel(int addr,int count,unsigned char *ch,unsigned int *vals);
extern int FSSendLongCommandSameByteWithChannel(int addr,int count,unsigned char *ch,unsigned char val);
extern int FSSendLongCommandSameWordWithChannel(int addr,int count,unsigned char *ch,unsigned int val);
extern int FSMoveTo(int id,int pos,int period);
extern int FSServoOnOff(int id,int onoff);

#define FSMoveToWithChannel(count,ch,pos) FSSendLongCommandWordWithChannel(30,count,ch,pos)
#define FSServoOnOffWithChannel(count,ch,onoff) FSSendLongCommandSameByteWithChannel(36,count,ch,onoff)

extern int FSSendMemoryRequest(int id, int addr, int count);
extern void FSClearResponse(void);
extern void FSWaitResponse(int len,int timeout);
extern int FSDecodeResponse(int *id,int *addr,int maxcount,unsigned char *data);
extern void FSCheckTimeout(void);


extern int FSResponseTimeOut;