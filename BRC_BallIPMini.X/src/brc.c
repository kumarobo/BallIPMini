// Balance Robot Controller

#include "brc.h"

// Hardware CONFIGRATION
// OSC, OSCPLL
_FOSC(POSCMD_EC /* External */ & OSCIOFNC_ON /*OSC2=IO*/ & IOL1WAY_OFF /* Enalble multiple reconfig */ & FCKSM_CSECME /* Clock switch & fail-safe */)
_FOSCSEL(FNOSC_PRIPLL /* PrimaryPLL */ & IESO_ON /* FRC startup */)

// WatchDog
_FWDT(FWDTEN_OFF /* DISABLE */ & WDTPOST_PS32768 /* postscaler */ & WDTPRE_PR128 /* prescaler */ & PLLKEN_ON /* wait PLL */ & WINDIS_OFF /* Window */)
// Poweron and I2C select
_FPOR(FPWRT_PWR64 /* Power-on reset, 64ms */ & BOREN_OFF /* Brown-out off */ & ALTI2C1_ON /* ASDA1/ASCK1 selected-enabled */)

// ICD debugging : nothing to do with
// _FICD()
// General memory / Aux flush memory : nothing to do with
//_FAS()
//_FGS()
        
void BRC_SetupClockPLL(void)
{
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    PLLFBD=BRC_FCYMHZ; // M 60 for FCY=60MH(FOSC120MHz), 60MIPS   70 for 70MHz
    CLKDIVbits.PLLPOST=0; // N2=2
    CLKDIVbits.PLLPRE=0; // N1=2
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
}

void BRC_SetupUSBClock(void)
{
    // see doc.70580C section 7.11
    ACLKCON3bits.SELACLK=1; // AuxPLL
    ACLKCON3bits.AOSCMD=0;  // disable AuxOSC
    ACLKCON3bits.ASRCSEL=1; // APLL src=primary OSC
    ACLKCON3bits.FRCSEL=0;  // APLL src=primary OSC
    ACLKCON3bits.APLLPOST=6;  // output divider ; /2 // 96MHz->48MHz
    ACLKCON3bits.APLLPRE=1;   // input divider ; /2  // 8MHz->4MHz
    ACLKDIV3=0x7;   // PLL divider ; 4MHz->96MHz = x24 
    // example; ACLKCON3 = 0x24C1; AOSCMD=0 ASRCSEL=1 FRCSEL=0 POST=b110 PRE=b001
    // example; ACLKDIV3 = 0x7;

    // EnablePLL
    ACLKCON3bits.ENAPLL=1;
}


void BRC_IORemap(void)
{
   	__builtin_write_OSCCONL(OSCCONL&(~0x40));  // unlock RP
    // SerialPort
    // ComRx:RP100
    // COmTx:RP101
   	RPINR18bits.U1RXR=RPIN_ComRx;   // UART 1 RX <- RP_ComRx
	RPOUT_ComTx=1;      // RP_ComTx <- UART 1 TX
	__builtin_write_OSCCONL(OSCCONL|0x40);  // lock RP
}

void BRC_InitializeLEDSW(void)
{
    TRIS_LEDRed=TRIS_OUT;
    TRIS_LEDGreen=TRIS_OUT;
    TRIS_LEDSW=TRIS_IN;
//    TRISDbits.TRISD0=0; // LED2  green
//    TRISDbits.TRISD11=0; // LED1  red
//    TRISDbits.TRISD8=1;  // SW
}


// Master clock counter using Timer 2
volatile long BRC_SysClock;

void BRC_InitializeSysClock(unsigned long Hz)
{
    IPC1bits.T2IP = 6; // priority; 0:disable 1:lowest 7:highest    
    IFS0bits.T2IF = 0;

    TMR2 = 0;
    T2CONbits.TON=0;   // stop
    T2CONbits.TSIDL=1; // stop while idle
    T2CONbits.TGATE=0; // gate disable
    T2CONbits.TCKPS=0; // prescaler 0=1:1 1=1:8 2=1:64 3=1:256
    T2CONbits.T32=0;   // 16bit count
    T2CONbits.TCS=0;   // internal clock FCY

    unsigned long div=BRC_FCYMHZ*1000l*1000l/Hz;
    if(div>(256l<<16)) div=255l<<16;
         if(div>( 64l<<16)) { T2CONbits.TCKPS=3; PR2=(unsigned int)((div+0x80)>>8); }
    else if(div>(  8l<<16)) { T2CONbits.TCKPS=2; PR2=(unsigned int)((div+0x20)>>6); }
    else if(div>(  1l<<16)) { T2CONbits.TCKPS=1; PR2=(unsigned int)((div+0x04)>>3); }
    else                    { T2CONbits.TCKPS=0; PR2=(unsigned int)( div   ); }

    IEC0bits.T2IE = 1;
    T2CONbits.TON=1;   // timer start
    
    BRC_SysClock=0;
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt( void )
{
    if (IFS0bits.T2IF)
    {
        IFS0bits.T2IF   = 0;
        BRC_SysClock++;
    }
}


// Output Compare Register List
#define _OCLISTITEM(a)                  \
  { &OC##a##R,    &OC##a##RS,           \
    &OC##a##CON1, &OC##a##CON2,         \
    (OC1CON1BITS *)(&OC##a##CON1bits),  \
    (OC1CON2BITS *)(&OC##a##CON2bits) }

typedef struct tagOCList
{
    volatile unsigned int *OCxR,*OCxRS,*OCxCON1,*OCxCON2;
    volatile OC1CON1BITS *OCxCON1bits;  // OCxCON1 is same for x=1 to 16 on MU806
    volatile OC1CON2BITS *OCxCON2bits;  // OCxCON2 is same for x=1 to 16 on MU806
} OCList;
static OCList BRC_OCList[17]= {
    { 0,0,0,0, },
    _OCLISTITEM( 1), _OCLISTITEM( 2), _OCLISTITEM( 3), _OCLISTITEM( 4),
    _OCLISTITEM( 5), _OCLISTITEM( 6), _OCLISTITEM( 7), _OCLISTITEM( 8),
    _OCLISTITEM( 9), _OCLISTITEM(10), _OCLISTITEM(11), _OCLISTITEM(12),
    _OCLISTITEM(13), _OCLISTITEM(14), _OCLISTITEM(15), _OCLISTITEM(16),
};

// Generate Motor Pulse by DDS, using Timer3, OC
// OutputCompare usage list
static int BRC_MPOCList[BRC_NMP]={BRC_MPOCLISTDEF};
// control structure
static struct BRC_MPS
{
	unsigned int phase;
	unsigned int inc;
	int sign;
} brc_mps[BRC_NMP] = {{0,0,0},{0,0,0},{0,0,0}};
static unsigned int BRC_DDSPERIOD=3000;

const int BRC_OC_RPOUTList[17]=
    {0, 
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c};

int BRC_OCxRPO(int ch)
{
    if((ch<1)||(ch>16)) return 0;
    return BRC_OC_RPOUTList[ch];
}

int BRC_OCxRPO_MPCH(int ch)
{
    if((ch<0)||(ch>=BRC_NMP)) return 0;
    return (BRC_OCxRPO(BRC_MPOCList[ch]));
}

void BRC_InitializeMotorPulse(void)
{
    int ch;
    OCList *oc;
    // calculate DDS base period
    BRC_DDSPERIOD=
       (unsigned int)
       (((BRC_FCYMHZ*1000l*1000l)*BRC_MPUNITHZ_INV)>>16);
  	// TimeBase Counter using Timer 3
	T3CON=0;
	TMR3=0;      // count
	PR3=BRC_DDSPERIOD;  // count max
	T3CONbits.TCKPS=0; // prescaler=1/1  // 16MHz
	T3CONbits.TON=1;   // start

    // Base cycle generator
    oc=BRC_OCList+BRC_MPBASEOC;
    *(oc->OCxCON1)=0;
    *(oc->OCxCON2)=0;
    oc->OCxCON1bits->OCTSEL=1; // Timer 3
    oc->OCxCON2bits->SYNCSEL=0x1f;
    *(oc->OCxR )=BRC_DDSPERIOD/2; 
    *(oc->OCxRS)=BRC_DDSPERIOD;
    oc->OCxCON1bits->OCM=5; // double compare continueous pulse 
    
    for(ch=0;ch<BRC_NMP;ch++)
    {
        oc=BRC_OCList+BRC_MPOCList[ch];
        *(oc->OCxCON1)=0;
        *(oc->OCxCON2)=0;
        oc->OCxCON1bits->OCTSEL=1; // Timer 3
        oc->OCxCON2bits->SYNCSEL=BRC_MPBASEOC;  // note: only 1-9 for MU806
        *(oc->OCxR )=(ch+1)*100; // dummy 
        *(oc->OCxRS)=BRC_DDSPERIOD;
        oc->OCxCON1bits->OCM=5; // double compare continueous pulse 

    }
    // interrupt enable
    _OC1IE=1; //IEC0bits.OC1IE=1;  
    _OC1IP=7; //IPC0bits.OC1IP=7;
}

static void BRC_DDSMain(void);

void __attribute__((__interrupt__, auto_psv)) _OC1Interrupt( void )
{
    if (IFS0bits.OC1IF)
    {
        IFS0bits.OC1IF   = 0;
		BRC_DDSMain();
    }
}
void BRC_DDSMain(void)
{
	int i;
    volatile unsigned int *OCxR;
	for(i=0;i<BRC_NMP;i++)
	{
        OCxR=BRC_OCList[BRC_MPOCList[i]].OCxR;
//		if((brc_mps[i].phase&0x8000)&&(((brc_mps[i].phase+brc_mps[i].inc)&0x8000)==0))
        if(brc_mps[i].phase>brc_mps[i].phase+brc_mps[i].inc)
		{  // round-up
			*OCxR=10;
		}
		else
		{
			*OCxR=BRC_DDSPERIOD+10;   // keep low
		}
		brc_mps[i].phase+=brc_mps[i].inc;
	}
}

void BRC_SetMotorSpeed(int ch,int vel)
{
	if((ch<0)&&(ch>=BRC_NMP)) return;
	if(vel<0)
	{
		if(brc_mps[ch].sign>0)
			brc_mps[ch].phase=0;
		brc_mps[ch].inc=-vel;
		brc_mps[ch].sign=-1;
#ifdef BRC_SetCWCCW
		BRC_SetCWCCW(ch,1);
#endif
	}
	else
	{
		if(brc_mps[ch].sign<0)
			brc_mps[ch].phase=0;
		brc_mps[ch].inc=vel;
		brc_mps[ch].sign=1;
#ifdef BRC_SetCWCCW
		BRC_SetCWCCW(ch,0);
#endif
	}
}

// LED Switch Board



static const unsigned char LSB_LEDpat[16]={  // DP:0x80
  /* 0:01110111 */ 0x77,
  /* 1:00010100 */ 0x14,
  /* 2:01101101 */ 0x6d,
  /* 3:00111101 */ 0x3d,
  /* 4:00011110 */ 0x1e,
  /* 5:00111011 */ 0x3b,
  /* 6:01111011 */ 0x7b,
  /* 7:00010101 */ 0x15,
  /* 8:01111111 */ 0x7f,
  /* 9:00111111 */ 0x3f,
  /* A:01011111 */ 0x5f,
  /* b:01111010 */ 0x7a,
  /* c:01101000 */ 0x68,
  /* d:01111100 */ 0x7c,
  /* E:01101011 */ 0x6b,
  /* F:01001011 */ 0x4b,
};

#define LSB_CLK BRC_LSB_CLK_LAT
#define LSB_RCK BRC_LSB_RCK_LAT
#define LSB_LSD BRC_LSB_LSD_LAT
#define LSB_SSD BRC_LSB_SSD_PORT

static void __attribute__((optimize("-O0"))) _shortdelay(void)
{
    int i;
    for(i=0;i<1;i++);
}

// __attribute__((optimize("-O0")))
static unsigned long __attribute__((optimize("-O0"))) BRC_LEDSwitchBoard_IO(unsigned long led)
{
    int i;
    unsigned long s=0;
    unsigned int w;
    LSB_CLK=0;
    LSB_RCK=0;
    _shortdelay();
    for(i=BRC_LSB_LB-1;i>=0;i--)
    {
        LSB_LSD=(led&(1<<i))?1:0;
        _shortdelay();
        LSB_CLK=1;
        _shortdelay();
        LSB_CLK=0;
    }
    _shortdelay();
    LSB_RCK=1;  _shortdelay();
    LSB_RCK=0;  _shortdelay();
    LSB_RCK=1;  _shortdelay();
    LSB_LSD=0;
    for(i=0;i<BRC_LSB_SB;i++)
    {
        s<<=1;
        s|=(LSB_SSD)?1:0;
        LSB_CLK=1; _shortdelay();
        LSB_CLK=0; _shortdelay();
    }
    /* bit swap */
    // return s; // ((s>>12)&0x0f0f)|((s>>4)&0xf0f0)|((s&0xff)<<16);
    w=(unsigned int)(s>>8);
    w=(w>>8)|(w<<8);
    w=((w&0xf0f0)>>4)|((w&0x0f0f)<<4);
    // LSB_RCK=0;
    return ((s&0xff)<<16)|w;
}



static struct _BRC_LSB_STATE {
    unsigned long segment;
    unsigned int leds;
    unsigned long sw;
    int leddigit;
} BRC_LSBState;

void BRC_LEDSwitchBoard_CycleTask(void)
{
    BRC_LSBState.leddigit=(BRC_LSBState.leddigit+1)&0x3;
    BRC_LSBState.sw=BRC_LEDSwitchBoard_IO(// test for bitmasil witchBoard_IO(BRC_LSBState.led);
            (1<<(BRC_LSBState.leddigit))|
            (BRC_LSBState.leds<<4)|
            (((BRC_LSBState.segment<<(BRC_LSBState.leddigit*8))&0xff000000)>>16));
}

  
bool BRC_LEDSwitchBoard_GetPush(void)
{
    return (BRC_LSBState.sw&0x10000l)?true:false;
}

unsigned int BRC_LEDSwitchBoard_GetSwitch(void)
{
    return (unsigned int)(BRC_LSBState.sw&0xffff);
}

extern unsigned long BRC_LEDSwitchBoard_GetSwitchFull(void)
{
    return BRC_LSBState.sw;
}


void BRC_LEDSwitchBoard_SetSegmentsRaw(unsigned long segments)
{
    BRC_LSBState.segment=segments;
}

void BRC_LEDSwitchBoard_SetHex(unsigned int v)
{
    int i;
    BRC_LSBState.segment=0;
    for(i=0;i<4;i++)
    {
        BRC_LSBState.segment=(BRC_LSBState.segment<<8)|
                 LSB_LEDpat[(v>>12)&0xf];
        v=v<<4;
    }
}

void BRC_LEDSwitchBoard_SetUIntZS(unsigned int v,int zs)
{
    int i;
    unsigned int vs;
    BRC_LSBState.segment=0;
    for(i=0;i<4;i++)
    {
        vs=v/10;
        if((v)||(i<zs))
            BRC_LSBState.segment=(BRC_LSBState.segment>>8)|
                     ((unsigned long)(LSB_LEDpat[v-vs*10])<<24);
        else
            BRC_LSBState.segment=(BRC_LSBState.segment>>8);
        v=vs;
    }
}

void BRC_LEDSwitchBoard_SetSIntZS(int v,int zs)
{
    if(v>9999) v=9999;
    if(v<-1999) v=-1999;
    if(v>=0)
        BRC_LEDSwitchBoard_SetUIntZS(v,zs);
    else
    {
        BRC_LEDSwitchBoard_SetUIntZS(-v,zs);
        BRC_LSBState.segment|=0x08000000l;
    }
}

void BRC_LEDSwitchBoard_SetDP(int dp)
{
    BRC_LSBState.segment|=(0x80l)<<(dp*8);
}


void BRC_LEDSwitchBoard_SetLED(unsigned int leds)
{
    BRC_LSBState.leds=leds&0xf;
}

void BRC_LEDSwitchBoard_SetLEDBits(int no,bool onoff)
{
    unsigned int mask=1<<no;
    BRC_LSBState.leds=(BRC_LSBState.leds&(~mask))|(onoff?mask:0);
}


void BRC_InitializeLEDSwitchBoard(void)
{
    BRC_LSB_CLK_TRIS=TRIS_OUT;
    BRC_LSB_RCK_TRIS=TRIS_OUT;
    BRC_LSB_LSD_TRIS=TRIS_OUT;
    BRC_LSB_SSD_TRIS=TRIS_IN;
    BRC_LSBState.segment=0;
    BRC_LSBState.leds=0;
    BRC_LSBState.sw=0;
    BRC_LSBState.leddigit=0;
    BRC_LEDSwitchBoard_CycleTask();
}







#if 0

void SPIOsetup(void)
{
	// setting for UART port
	__builtin_write_OSCCONL(OSCCONL&(~0x40));  // unlock RP
	// UART
	// TX SL(URAT1)=17(RP8)
	// RX SL(URAT1)=18(RP9)
	//RPINR18bits.U1RXR=9;   // UART 1 RX <- RP9
	//RPOR4bits.RP8R=3;      // RP8 <- UART 1 TX
	RPOR1bits.RP2R=3;     // RP2(pin6) <- UART 1 TX
	RPINR18bits.U1RXR=3;  // UART 1 RX <- RP3(pin7) , serial port connector
	TRISBbits.TRISB2=0;
	TRISBbits.TRISB3=1;
	AD1PCFGbits.PCFG4=1;  // AN4 digital configuration
	AD1PCFGbits.PCFG5=1;  // AN5 digital configuration

	//LATBbits.LATB2=1;
	//LATBbits.LATB3=1;

	// servo connected: RP4(pin11), RP7(pin16), RP8(pin17), RP9(pin18)
	// CLK1:RP7, CLK2:RP8 CLK3:RP4
	//RPOR4bits.RP9R=18; // RP9 <- OC1(18)
	TRISBbits.TRISB9=0; //RP9, RB9, Enable
	RPOR3bits.RP7R=19; // RP7 <- OC2(19)
	RPOR4bits.RP8R=20; // RP8 <- OC3(20)
	RPOR2bits.RP4R=21; // RP4 <- OC4(21)  // SOSCSEL_IO@CONFIG_3 required
	//RPOR2bits.RP4R=21; // RP4 <- OC4(21)  // SOSCSEL_IO@CONFIG_3 required
	//RPOR3bits.RP7R=19; // RP7 <- OC2(19)
	//RPOR4bits.RP8R=20; // RP8 <- OC3(20)
	//RPOR4bits.RP9R=18; // RP9 <- OC1(18)
	TRISAbits.TRISA0=0; // CWCCW1
	TRISAbits.TRISA1=0; // CWCCW2
	AD1PCFGbits.PCFG0=1;  // AN4 digital configuration
	AD1PCFGbits.PCFG1=1;  // AN5 digital configuration
	TRISBbits.TRISB15=0; // CWCCW3


	__builtin_write_OSCCONL(OSCCONL|0x40);  // lock RP
}

// DDSPeriod=16384Hz, 16MHz/16384=976.56
#define DDSPERIOD 977

void SetupServoPWM(void)
{
}








void InitializeServoPad(void)
{
	int i;
	SPIOsetup();
	SetupServoPWM();
	InitializeSerialLoop(1,SLB57600,31,"SMDCUSBPad20151208");
	for(i=0;i<16;i++) { RegFileS[i]=0; RegFileL[i]=0; }
	for(i=0;i<10;i++) SLNop();
	//InitializeXBEEAPI(0,XBB9600);
	//XBEEPortOnOff(xbeedest,1,0);
	//XBEEPortOnOff(xbeedest,0,0);
	/*SetDDS(0,100);
	SetDDS(1,256);
	SetDDS(2,22000);
	SMDC_ENABLE=1;*/
	
}

#define J2D(a) ( (int)(a-0x80) * 200)
static BYTE _axisold[4];
static WORD _buttonold;
static BYTE _repcycle;

void ServoPadUpdate(BYTE *axis,WORD button)
{
	int i;
	for(i=0;i<4;i++)
	{
		if(_axisold[i]!=axis[i]) 
		{
			_repcycle=0;
			_axisold[i]=axis[i];
		}
	}
	if(_buttonold!=button)
	{
		_repcycle=0;
		_buttonold=button;
	}

/*	SetServoPWM(0,J2S(axis[0]));
	SetServoPWM(1,J2S(axis[1]));
	SetServoPWM(2,J2S(axis[2]));
	SetServoPWM(3,J2S(axis[3]));
*/
//	SetDDS(0,J2D(axis[0]));
//	SetDDS(1,J2D(axis[1]));
//	SetDDS(2,J2D(axis[2]));

	SetDDS(0,(-((long)J2D(axis[1]))*256+((long)J2D(axis[0]))*  0+((long)J2D(axis[3]))*200)>>9);
	SetDDS(1,(+((long)J2D(axis[1]))*128+((long)J2D(axis[0]))*222+((long)J2D(axis[3]))*200)>>9);
	SetDDS(2,(+((long)J2D(axis[1]))*128-((long)J2D(axis[0]))*222+((long)J2D(axis[3]))*200)>>9);

	
	if(button&0x00f)
	{
		SMDC_ENABLE=1;
	}
	else
	{
		SMDC_ENABLE=0;
	}

	if(_repcycle==0)
	{
		SLSet32(SLBroadcast,1,2000);
		SLSet16(SLBroadcast,1,3);

		SLReply32(63,8,(((DWORD)(axis[0]))<<24)|(((DWORD)(axis[1]))<<16)|
					(((WORD)(axis[2]))<<8)|(axis[3]));
		SLReply16(63,8,button);
		_repcycle=10;
	}

	_repcycle--;
}


void ServoPadMain(void)
{
	RegFileL[0]++;
}
#endif