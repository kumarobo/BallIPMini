// Futaba Serial Servo Interface for dsPIC
// based on Plain Serial
// by Kumagai, M.

#include "FutabaServo.h"
#include "brc.h"


// work area including ring buffer
volatile static SIWork siw;

static void InitializeSerialHardware(int baudrate);
static void SerialInvokeTransmit(void);

void InitializeSerial(int baudrate)
{
    SerialClearTxBuffer();
    SerialClearRxBuffer();
    InitializeSerialHardware(baudrate);
}

int SerialGetTxCount(void)
{
	if(siw.twp >= siw.trp) return siw.twp-siw.trp;
	else return siw.twp-siw.trp+SITXBLEN;
}
int SerialGetRxCount(void)
{
	if(siw.rwp >= siw.rrp) return siw.rwp-siw.rrp;
	else return siw.rwp-siw.rrp+SIRXBLEN;
}

int SerialGetTxSpace(void)
{
    return SITXBLEN-1-SerialGetTxCount();
}

int SerialGetRxSpace(void)
{
    return SIRXBLEN-1-SerialGetRxCount();
}

void SerialClearTxBuffer(void)
{
    siw.twp=siw.trp=0;
    siw.fsc_txlen=0;
    siw.fsrxw_len=0; siw.fsrxw_to=0;
}
void SerialClearRxBuffer(void)
{
    siw.rwp=siw.rrp=0;
}

int SerialWrite(unsigned char *data,int size)
{
    int i;
    if(size>SerialGetTxSpace()) // not enough
        return 0;
    for(i=0;i<size;i++)
    {
        siw.txb[siw.twp]=*data;
        siw.twp++;  if(siw.twp>=SITXBLEN) siw.twp-=SITXBLEN;
        data++;
    }
    SerialInvokeTransmit();
    return i;
}

int SerialWritePartial(unsigned char *data,int size)
{
    if(size>SerialGetTxSpace())
    {
        size=SerialGetTxSpace();
    }
    return SerialWrite(data,size);
}

int SerialRead(unsigned char *data,int size)
{
    if(size>SerialGetRxCount()) // not enough
        return 0;
    int i;
    for(i=0;i<size;i++)
    {
        *data=siw.rxb[siw.rrp];
        siw.rrp++;  if(siw.rrp>=SIRXBLEN) siw.rrp-=SIRXBLEN;
        data++;
    }
    return i;
}

int SerialReadPartial(unsigned char *data,int size)
{
    if(size>SerialGetRxCount())
    {
        size=SerialGetRxCount();
    }
    return SerialRead(data,size);
}

int SerialReadPartialNonDestructive(unsigned char *data,int size)
{
    int p=siw.rrp;
    if(size>SerialGetRxCount())
    {
        size=SerialGetRxCount();
    }
    int r=SerialRead(data,size);
    siw.rrp=p;
    return r;
}

// ======================================================
// hardware interfaces

#define EnableUARTInt(a)  {  EnableIntU2RX;  EnableIntU2TX; }
#define DisableUARTInt(a) { DisableIntU2TX; DisableIntU2RX; }

static void SerialInvokeTransmit(void)
{
    if(U2STAbits.TRMT)
		IFS1bits.U2TXIF = 1;
}

#define _SIW_TRP_INC_ROUND { siw.trp++; if(siw.trp>=SIRXBLEN) siw.trp-=SIRXBLEN; }

/* Interrupt functions ====================================== */
// interrupt, auto_psv or no_auto_psv  refer google!
void __attribute__((__interrupt__, auto_psv)) _U2TXInterrupt(void)
{  
	IFS1bits.U2TXIF = 0;
    unsigned char cmd;
    
    if((siw.fsrxw_len!=0)&&(*siw.fsrxw_len>0))  // wait reception
        return;
            
    if(siw.fsc_txlen==0)
    {
        if(siw.twp==siw.trp)
            FS_485Disable();
        else  
        {  // check new command
            cmd=siw.txb[siw.trp];
            _SIW_TRP_INC_ROUND;
            if(cmd==FSCTX)
            {
                FS_485Enable();
                siw.fsc_txlen=siw.txb[siw.trp];
                _SIW_TRP_INC_ROUND;
            }
            else if(cmd==FSCRXCLEAR)
            {
                SerialClearRxBuffer();
            }
            else if(cmd==FSCRXWAIT)
            {
                siw.fsrxw_len=(unsigned char*)&(siw.txb[siw.trp]);
                _SIW_TRP_INC_ROUND;
                siw.fsrxw_to= (unsigned char*)&(siw.txb[siw.trp]);
                _SIW_TRP_INC_ROUND;
                FS_485Disable();
            }
        }
    }
    
    if(siw.fsc_txlen>0)
    {
    	while((siw.twp!=siw.trp)&&(!U2STAbits.UTXBF)&&(siw.fsc_txlen>0))
        {
            U2TXREG=siw.txb[siw.trp];
            _SIW_TRP_INC_ROUND;
            siw.fsc_txlen--;
        }
    }
	return ;
}

void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;
    while(U2STAbits.URXDA)
	{
        siw.rxb[siw.rwp]=U2RXREG;
        if(SerialGetRxSpace()>0)
        {
            siw.rwp++;
            if(siw.rwp>=SIRXBLEN) siw.rwp-=SIRXBLEN;
            if((siw.fsrxw_len!=0)&&(*siw.fsrxw_len>0))  // wait reception
            {
                *siw.fsrxw_len = *siw.fsrxw_len-1;
                if(*siw.fsrxw_len==0)  // received enough
                {
                    siw.fsrxw_len=0;
                    siw.fsrxw_to=0;
                    SerialInvokeTransmit(); // restart transmit
                }
            }
        }
        else
        {} // buffer overrun
	}
}

static void InitializeSerialHardware(int baudrate)
{
	// -> dsPIC30F_dsPIC33F_PIC24H_UART_Help.htm
	// -> SecI 70188.pdf
	// -> uart.h
	unsigned int U2MODEvalue, U2STAvalue, baudvalue;
	// stop it
	CloseUART2();
	// interrupt setting
	// _EN: enable, _DIS: disable, PR7: highest RP1:lowest
	// see also SecIII 70214B.pdf
	ConfigIntUART2(UART_RX_INT_EN  & UART_RX_INT_PR4 &
                   UART_TX_INT_EN  & UART_TX_INT_PR2);
	// modes
    U2MODEvalue= UART_EN & UART_IDLE_CON & UART_IrDA_DISABLE &
                 UART_MODE_SIMPLEX & UART_UEN_00 & 
                 /*UART_RX_TX &, N/A */
                 UART_DIS_WAKE & UART_DIS_LOOPBACK & 
                 UART_DIS_ABAUD & UART_BRGH_SIXTEEN & UART_UXRX_IDLE_ONE &
                 UART_NO_PAR_8BIT & UART_1STOPBIT; // ONLY IMPORTANT SETTING
    U2STAvalue=  UART_INT_TX_LAST_CH & //BUF_EMPTY & 
                 UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED &
                 UART_TX_ENABLE & UART_INT_RX_CHAR &
                 UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR;
    // Fcy=64,000,000Hz
	// baud = (Fcy / 16) /(vaudvalue+1)
	// baudvalue= (Fcy / 16) / baud -1;   64,000/16/115.2=34.7
	// 34: baud=114.28 99.2% 
	baudvalue = 416; // 9600bps
    if(baudrate==SIB115200) baudvalue = 34;
    if(baudrate==SIB57600) baudvalue = 68;
    OpenUART2(U2MODEvalue, U2STAvalue, baudvalue);	
	//EnableIntU2TX;
    EnableUARTInt();
}



// ===========================================
// Futaba Servo
void InitializeFutabaServo(int baudrate)
{
    StartIORemap();
  	RPINR19bits.U2RXR=FS_RPIN;   // UART 2 RX <- RPIN; def in FutabaServo.h
	FS_RPOUT=3;      // RPOUT <- UART 2 TX
    EndIORemap();
    FS_TRISOE=TRIS_OUT;
    InitializeSerial(baudrate);
}

int FSSendShortCommand(int id,int addr,int count,unsigned char *data)
{
	unsigned char buff[10],sum;
	int i;

    if(SerialGetTxSpace()<count+10) return -1; // not enough in TxBuffer

    buff[0]=0xfa; buff[1]=0xaf;  // header
	buff[2]=id; // servo id
	buff[3]=0x00; //flag
	buff[4]=addr;  // addr of on/off
	buff[5]=count; // len
	buff[6]=1; // servo count
	//for(i=0;i<count;i++)
	//	buff[7+i]=data[i];
	//checksum
	sum=0;
	for(i=2;i<7;i++)
		sum^=buff[i];
    for(i=0;i<count;i++)
        sum^=data[i];
	buff[7]=sum;
    
    buff[8]=FSCTX;  buff[9]=count+8;
    SerialWrite(buff+8,2);  // TX command (count+8)

    SerialWrite(buff,7);
    SerialWrite(data,count);
    SerialWrite(buff+7,1);
	return 0;
}

int FSSendLongCommand(int addr,int count,int byteseach,unsigned char *data)
{
    
	unsigned char buff[10],sum;
	int i,l=count*byteseach;
    
    if(SerialGetTxSpace()<l+10) return -1;
    
	buff[0]=0xfa; buff[1]=0xaf;  // header
	buff[2]=0; // servo id=0
	buff[3]=0x00; //flag=0
	buff[4]=addr;  // addr
	buff[5]=byteseach; // len
	buff[6]=count; // servo count
	//for(i=0;i<l;i++)
	//	buff[7+i]=data[i];
	//checksum
	sum=0;
	for(i=2;i<7;i++)
		sum^=buff[i];
    for(i=0;i<l;i++)
        sum^=data[i];
	buff[7]=sum;
    
    buff[8]=FSCTX;  buff[9]=l+8;
    SerialWrite(buff+8,2);  // TX command (count+8)

    SerialWrite(buff,7);   // header
    SerialWrite(data,l);   // data body
    SerialWrite(buff+7,1); // checksum
	return 0;
}

int FSSendLongCommandByteWithChannel(int addr,int count,unsigned char *ch,unsigned char *vals)
{
	unsigned char data[32];
    int i;
    if(count>=16) return -1;
	for(i=0;i<count;i++)
	{
		data[i*2+0]=ch[i];
		data[i*2+1]=vals[i];
	}
	return FSSendLongCommand(addr,count,2,data);
}

int FSSendLongCommandWordWithChannel(int addr,int count,unsigned char *ch,unsigned int *vals)
{
	unsigned char data[48];
    int i;
    if(count>=16) return -1;
	for(i=0;i<count;i++)
	{
		data[i*3+0]=ch[i];
		data[i*3+1]=(vals[i]   )&0xff; // low
		data[i*3+2]=(vals[i]>>8)&0xff; // high
	}
	return FSSendLongCommand(addr,count,3,data);
}

int FSSendLongCommandSameByteWithChannel(int addr,int count,unsigned char *ch,unsigned char val)
{
	unsigned char data[32];
    int i;
    if(count>=16) return -1;
	for(i=0;i<count;i++)
	{
		data[i*2+0]=ch[i];
		data[i*2+1]=val;
	}
	return FSSendLongCommand(addr,count,2,data);
}

int FSSendLongCommandSameWordWithChannel(int addr,int count,unsigned char *ch,unsigned int val)
{
	unsigned char data[48];
    int i;
    if(count>=16) return -1;
    unsigned char l=(val   )&0xff, h=(val>>8)&0xff;
	for(i=0;i<count;i++)
	{
		data[i*3+0]=ch[i];
		data[i*3+1]=l; // low
		data[i*3+2]=h; // high
	}
	return FSSendLongCommand(addr,count,3,data);
}

void FSClearResponse(void)
{
    unsigned char buff[1];
    if(SerialGetTxSpace()<1) return; // not enough in TxBuffer
    buff[0]=FSCRXCLEAR;
    SerialWrite(buff,1);  // CLEAR command
}

void FSWaitResponse(int len,int timeout)
{
    unsigned char buff[3];
    if(SerialGetTxSpace()<3) return; // not enough in TxBuffer
    buff[0]=FSCRXWAIT;
    buff[1]=(unsigned char)(len&0xff);
    buff[2]=(unsigned char)(timeout&0xff);
    SerialWrite(buff,3);  // RXWAIT command
}

void FSCheckTimeout(void)
{
    if((siw.fsrxw_to!=0)&&(*siw.fsrxw_to!=0))
    {
        *siw.fsrxw_to = *siw.fsrxw_to-1;
        if(*siw.fsrxw_to==0) // timedout
        {
            siw.fsrxw_len=0;
            siw.fsrxw_to=0;
            SerialInvokeTransmit(); // restart transmit
        }
    }
}

int FSResponseTimeOut=5;

int FSSendMemoryRequest(int id, int addr, int count)
{
    unsigned char buff[10],sum;
	int i;

    if(SerialGetTxSpace()<10+3) return -1; // not enough in TxBuffer

	buff[0]=0xfa; buff[1]=0xaf;  // header
	buff[2]=id; // servo id
	buff[3]=0x0f; //flag
	buff[4]=addr;  // addr of on/off
	buff[5]=count; // len
	buff[6]=0; // servo count
	//checksum
	sum=0;
	for(i=2;i<7;i++)
		sum^=buff[i];
	buff[7]=sum;
    
    buff[8]=FSCTX;  buff[9]=8;
    SerialWrite(buff+8,2);  // TX command (count+8)

    SerialWrite(buff,8);
    
    FSWaitResponse(8+count,FSResponseTimeOut);
	return 0;
}

int FSDecodeResponse(int *id,int *addr,int maxcount,unsigned char *data)
{
    unsigned char buff[8],sum;
    int i,l;
    // check header part
    if(SerialReadPartialNonDestructive(buff,7)<7) return -1;  // not enough
    if((buff[0]!=0xfd)||(buff[1]!=0xdf)) return -2;  // header magic error
    if(buff[6]!=1) return -3; // must be 1 in return packet
    // total length = 8+buff[5]
    l=buff[5];
    if(SerialGetRxCount()<8+l) return -4; // not enough length
    if(maxcount<l) return -5; // not enough receive buff
    // read
    SerialRead(buff,7);
    SerialRead(data,l);
    SerialRead(buff+7,1);    // checksum
    // cehcksum
    sum=0;
    for(i=2;i<7;i++)
		sum^=buff[i];
    for(i=0;i<l;i++)
		sum^=data[i];
	if(buff[7]!=sum) return -6;
    *id=buff[2];
    *addr=buff[3];
    return l;
}



// shortcut functions

int FSMoveTo(int id,int pos,int period)
{
    unsigned char buff[4];
	buff[0]=pos   &0xff;  buff[1]=(pos   >>8)&0xff;
	buff[2]=period&0xff;  buff[3]=(period>>8)&0xff;
	return FSSendShortCommand(id,30,4,buff);
}

int FSServoOnOff(int id,int onoff)
{
    unsigned char of=onoff?1:0;
	return FSSendShortCommand(id,36,1,&of);
}

