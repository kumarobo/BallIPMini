/* Serial loop definitions */
/*
 *
 * pre-defines before include 
 *   REGFILEC (16) : number of RegFileS, RegFileL
 *   ENABLE_FORWARD (N/A) : packet forward enable (recommended)

 *   USE_BULK (N/A) : define if using bulk transfer
 *   BULKBUFFERSIZE(16) : bulk transfer buffer size

 *   USE_MEMEXT (N/A) : define if using memory access extension, not checked

 *   SLTXBLEN (150) : transmit buffer size (depend on memory size)

 * accessible variables
 *   RegFileS[REGFILEC] 16bit reg unsigned 
 *   RegFileL[REGFILEC] 32bit reg unsigned
 *   BulkBuffer[BULKBUFFERSIZE] 

 * device-dependent functions
 *   int SLInitializeUART(SLWork *slw,int baudrate);
 *   #define InvokeTransmit // wakeup transmitter, macro or function define
 *   #define EnableUARTInt  // enable interrupt
 *   #define DisableUARTInt // disable interrupt
 */
#ifndef __SLNODE_H
#define __SLNODE_H

// Device Dependence Header; modified for each hardware
#include "slnode11_dspicEP.h"


#ifndef REGFILEC
#define REGFILEC 16
#endif
extern volatile unsigned int RegFileS[REGFILEC];
extern volatile unsigned long RegFileL[REGFILEC];


/* default version is SL1.1 */
#ifndef USESL10
#define USESL11
#endif 

// transmit buffer size (as ring buffer)
#ifndef SLTXBLEN
#define SLTXBLEN 256
#endif

// receive buffer size (for one packet)
#ifndef SLRXBLEN
#define SLRXBLEN 258
#endif

// packet transmit wait buffer size (for several packet)
#ifndef SLPKBLEN
#define SLPKBLEN SLTXBLEN
#endif


#ifdef USE_BULK
#ifndef BULKBUFFSIZE
#define BULKBUFFSIZE 16
#endif
extern volatile unsigned int BulkBuffer[BULKBUFFSIZE];
#endif


#define SLRS_START 2
#define SLRS_SKIP  3
#define SLRS_RECV  4

/* baud rate */
#define SLB115200 1152
#define SLB57600   576

/* broadcast destination ID */
#define SLBroadcast 0x80


typedef struct _SLWork
{
	/* slnode id */
	int nid;
	char *profile;
	/* hardware channel number */
	int ch;

	/* transmit */
	unsigned char txb[SLTXBLEN];
	volatile int twp,trp; /* ring counter */

	/* receive */
	unsigned char rxb[SLRXBLEN];
	int rxc; /* point counter */
	char rs; /* receive state */
	char tf; /* trans through flag */

	/* packet stock before transmit */
	unsigned char pkb[SLPKBLEN+2];  /* 2=spair */
	volatile int pkc; /* point counter */
} SLWork;


/* hardware dependent functions ============ */
extern int SLInitializeUART(SLWork *slw,int baudrate);
extern void SLTransmit(SLWork *slw);
/* hardware dependent macro/functions */
/* extern void InvokeTransmit(SLWork *slw); 
   #define InvokeTransmit(slw) */
/* EnableUARTInt(slw) */
/* DisableUARTInt(slw) */

/* public function =========================*/
/* initialize */
extern int InitializeSerialLoop(int ch,int baudrate,
                                int nodeid,char *profile);
/* process entrance from RxInt */
extern void SLReceiveByte(unsigned char byte);

extern void SLNop (void);
extern void SLNop4(void);
extern void SLSet16  (unsigned short dest,unsigned short reg,unsigned short val);
extern void SLSet32  (unsigned short dest,unsigned short reg,unsigned  long val);
extern void SLReply16(unsigned short dest,unsigned short reg,unsigned short val);
extern void SLReply32(unsigned short dest,unsigned short reg,unsigned  long val);
extern void SLGet16  (unsigned short dest,unsigned short reg);
extern void SLGet32  (unsigned short dest,unsigned short reg);
extern void SLReplyProfile(int dest,char *s);

#endif /* __SLNODE_H */
