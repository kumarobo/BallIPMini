// Plain Serial Interface for dsPIC
// by Kumagai, M.

#include "plain_serial.h"


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
}
void SerialClearRxBuffer(void)
{
    siw.rwp=siw.rrp=0;
}

extern int SerialWrite(unsigned char *data,int size)
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

extern int SerialWritePartial(unsigned char *data,int size)
{
    if(size>SerialGetTxSpace())
    {
        size=SerialGetTxSpace();
    }
    return SerialWrite(data,size);
}

extern int SerialRead(unsigned char *data,int size)
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

extern int SerialReadPartial(unsigned char *data,int size)
{
    if(size>SerialGetRxCount())
    {
        size=SerialGetRxCount();
    }
    return SerialRead(data,size);
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

/* Interrupt functions ====================================== */
// interrupt, auto_psv or no_auto_psv  refer google!
void __attribute__((__interrupt__, auto_psv)) _U2TXInterrupt(void)
{  
	IFS1bits.U2TXIF = 0;
	while((siw.twp!=siw.trp)&&(!U2STAbits.UTXBF))
	{
		U2TXREG=siw.txb[siw.trp];
		siw.trp++;
		if(siw.trp>=SIRXBLEN) siw.trp-=SIRXBLEN;
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
	CloseUART1();
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
