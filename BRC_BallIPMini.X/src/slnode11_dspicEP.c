/* Serial Loop device dependent source */

/* for dsPIC33EP
 *
 * Only support UART1
 */

#include <p33Exxxx.h>
#include "slnode.h"

/* Device dependent work ================== */
void InvokeTransmit(SLWork *slw)
{
	if(U1STAbits.TRMT)
		IFS0bits.U1TXIF = 1;
}

static SLWork *U1slw;

/* Interrupt functions ====================================== */
// interrupt, auto_psv or no_auto_psv  refer google!
void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void)
{  
	IFS0bits.U1TXIF = 0;
#ifdef DEBUG_SLTI
	DEBUG_SLTI=1;
#endif	
	while((U1slw->twp!=U1slw->trp)&&(!U1STAbits.UTXBF))
	{
		U1TXREG=U1slw->txb[U1slw->trp];
		U1slw->trp++;
		if(U1slw->trp>=SLTXBLEN) U1slw->trp=0;
	}
#ifdef DEBUG_SLTI
	DEBUG_SLTI=0;
#endif	
	return ;
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;
	while( DataRdyUART1())
	{
		SLReceiveByte(ReadUART1()); /* SLdecode */
	}
}  

int SLInitializeUART(SLWork *slw,int baudrate)
{
	// int ch=slw->ch;
	U1slw=slw;

	// -> dsPIC30F_dsPIC33F_PIC24H_UART_Help.htm
	// -> SecI 70188.pdf
	// -> uart.h
	unsigned int U1MODEvalue, U1STAvalue, baudvalue;
	// stop it
	CloseUART1();
	// interrupt setting
	// _EN: enable, _DIS: disable, PR7: highest RP1:lowest
	// see also SecIII 70214B.pdf
	ConfigIntUART1(UART_RX_INT_EN  & UART_RX_INT_PR4 &
                   UART_TX_INT_EN  & UART_TX_INT_PR2);
	// modes
    U1MODEvalue= UART_EN & UART_IDLE_CON & UART_IrDA_DISABLE &
                 UART_MODE_SIMPLEX & UART_UEN_00 & 
                 /*UART_RX_TX &, N/A */
                 UART_DIS_WAKE & UART_DIS_LOOPBACK & 
                 UART_DIS_ABAUD & UART_BRGH_SIXTEEN & UART_UXRX_IDLE_ONE &
                 UART_NO_PAR_8BIT & UART_1STOPBIT; // ONLY IMPORTANT SETTING
    U1STAvalue=  UART_INT_TX_LAST_CH & //BUF_EMPTY & 
                 UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED &
                 UART_TX_ENABLE & UART_INT_RX_CHAR &
                 UART_ADR_DETECT_DIS & UART_RX_OVERRUN_CLEAR;
    // Fcy=64,000,000Hz
	// baud = (Fcy / 16) /(vaudvalue+1)
	// baudvalue= (Fcy / 16) / baud -1;   64,000/16/115.2=34.7
	// 34: baud=114.28 99.2% 
	baudvalue = 416; // 9600bps
    if(baudrate==SLB115200) baudvalue = 34;
    if(baudrate==SLB57600) baudvalue = 68;
    OpenUART1(U1MODEvalue, U1STAvalue, baudvalue);	
	EnableIntU1TX;
	return 0;
}
