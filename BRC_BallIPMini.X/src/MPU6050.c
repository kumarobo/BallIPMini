/*
 * File:   MPU6050.c
 * Author: kumagai
 *
 * Created on 2015/12/28, 1:34
 */

#include "xc.h"

// MPU 6050 main
// #include <p33Exxxx.h>
#include "i2c.h"
#include "MPU6050.h"



// http://forum.sparkfun.com/viewtopic.php?f=4&t=24099
static void I2CInitialize(void)
{
	OpenI2C1(I2C1_ON &       /*I2C module enabled */
             I2C1_IDLE_CON & /*continue I2C module in Idle mode */
             I2C1_CLK_REL &  /*release clock */
             I2C1_IPMI_DIS & // /*IPMI mode enabled */
             I2C1_7BIT_ADD &  /*I2CADD is 7-bit address */
             I2C1_SLW_DIS &  // /*Disable Slew Rate Control for 100KHz */
             I2C1_SM_EN &   /*Disable SM bus specification */
             I2C1_GCALL_DIS & /*Disable General call address. */
             I2C1_STR_DIS &   /*disable clock stretching */
             I2C1_ACK &   //   /*Transmit 1 to send NACK as acknowledge*/
             I2C1_ACK_DIS &    /*Initiate Acknowledge sequence */
             I2C1_RCV_DIS &    /*Enable receive mode */
             I2C1_STOP_DIS &   /*Initiate Stop sequence */
             I2C1_RESTART_DIS & /*Initiate Restart sequence */
             I2C1_START_DIS,   /*Initiate Start sequence */
			 150 /*400kHz-64MHz*/ 
			); 
}
/* I2C clock: TSCL(400kHz)=2.5e-6, BRG=(TSCL-120e-9)*Tcy-2
   Tcy 40Mhz: 95.2-2  Tcy 64MHz: 152-2 */

static void I2CSendDataByte(unsigned char reg,unsigned char data)
{
	StartI2C1(); //Send the Start Bit
	IdleI2C1(); //Wait to complete

	MasterWriteI2C1(IMUWADDR);	IdleI2C1(); //  while(I2C1STATbits.ACKSTAT); 
	MasterWriteI2C1(reg);	IdleI2C1();  // while(I2C1STATbits.ACKSTAT); 
	MasterWriteI2C1(data); 	IdleI2C1(); //Wait to complete
	StopI2C1();
} //SendData

static unsigned char I2CReceiveDataByte(unsigned char reg)
{
	unsigned char r;

	StartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMUWADDR);	IdleI2C1();
	MasterWriteI2C1(reg);	IdleI2C1();
	RestartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMURADDR);	IdleI2C1();
	r=MasterReadI2C1();

	NotAckI2C1();	StopI2C1();

	return r;
}

static unsigned int I2CReceiveDataWord(unsigned char reg)
{
	unsigned int r1,r2;

	StartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMUWADDR);	IdleI2C1();
	MasterWriteI2C1(reg);	IdleI2C1();
	RestartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMURADDR);	IdleI2C1();
	r1=MasterReadI2C1();
	AckI2C1();	IdleI2C1();
	r2=MasterReadI2C1();
	NotAckI2C1();	IdleI2C1();

	StopI2C1();

	return (r1<<8)|r2;
}

static unsigned int I2CReceiveDataWordSwap(unsigned char reg)
{
	unsigned int r1,r2;

	StartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMUWADDR);	IdleI2C1();
	MasterWriteI2C1(reg);	IdleI2C1();
	RestartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMURADDR);	IdleI2C1();
	r1=MasterReadI2C1();
	AckI2C1();	IdleI2C1();
	r2=MasterReadI2C1();
	NotAckI2C1();	IdleI2C1();

	StopI2C1();

	return (r2<<8)|r1;
}

// delay, tick resolution is supposed to be 20kHz
// implement something similar function
static void Delay(int tick)
{	
	volatile long ns;
/*	ns=PWMsysclock+tick;
	while(ns>PWMsysclock)
		;*/
    for(ns=0;ns<tick*1000l*10;ns++)
        ;
}
static void SDelay(void)
{	
	volatile long ns;
/*	ns=PWMsysclock+tick;
	while(ns>PWMsysclock)
		;*/
    for(ns=0;ns<100;ns++)
        ;
}

IMUDataStruct imu;

void SetupMPU6050(void)
{
	int i;
	// device reset, sleep release
	Delay(100); I2CSendDataByte(0x6b,0x80);
	Delay(100); I2CSendDataByte(0x6b,0x00);
	Delay(100); I2CSendDataByte(0x6b,0x80);
	Delay(100); I2CSendDataByte(0x6b,0x00);

	// Data sampling setting
	//Delay(1);   I2CSendDataByte(0x19,0x03); // sampling rate setting, 1kHz/(3+1)=250Hz
	//Delay(1);   I2CSendDataByte(0x1a,0x01); // 00 000 000 / xx [extsync] [dlfp] / digital LPF setting, 1kHz-184Hz band
	Delay(1);   I2CSendDataByte(0x19,0x01); // sampling rate setting, 1kHz/(1+1)=500Hz
	Delay(1);   I2CSendDataByte(0x1a,0x01); // 00 000 000 / xx [extsync] [dlfp] / digital LPF setting, 1kHz-184Hz band
	//Delay(1);   I2CSendDataByte(0x19,0x0f); // sampling rate setting, 8kHz/(1+15)=500Hz
	//Delay(1);   I2CSendDataByte(0x1a,0x00); // 00 000 000 / xx [extsync] [dlfp] / digital LPF setting, 8kHz-non band
	Delay(1);   I2CSendDataByte(0x1b,0x00); // 000 00 000/ [STx][y][z] [FS] 000 / gyro scale setting, 250deg/s
	Delay(1);   I2CSendDataByte(0x1c,0x00); // 000 00 000/ [STx][y][z] [FS] 000 / acc scale setting, 2g

	// FIFO setting
	Delay(1);   I2CSendDataByte(0x6a,0x00); // 0 0 00 - 000 / - [FIFOen] [I2CMST][I2CDIS]- [FIFOreset][I2CMSTrst][SIGreset] / disable FIFO
	Delay(2);   I2CSendDataByte(0x6a,0x04); // 0 0 00 - 100 / - [FIFOen] [I2CMST][I2CDIS]- [FIFOreset][I2CMSTrst][SIGreset] / disable FIFO
	Delay(2);   I2CSendDataByte(0x6a,0x40); // 0 1 00 - 000 / - [FIFOen] [I2CMST][I2CDIS]- [FIFOreset][I2CMSTrst][SIGreset] / enable FIFO
	Delay(2);	I2CSendDataByte(0x23,0xf8); // 1 111 1 000 [temp][gyro x][y][z] [accxyz] [slv2][1][0] / FIFO mask
	imu.FIFOdataN=7;

	imu.FIFOc=0;
	imu.GyroAngleX=imu.GyroAngleY=0;
	imu.GyroAVX=imu.GyroAVY=0;
	imu.AccAngleX=imu.AccAngleY=0;
	imu.CGAngleX=imu.CGAngleY=0;
	imu.CGFltX=imu.CGFltY=0;
	imu.zofc=0;
	for(i=0;i<imu.FIFOdataN;i++)
		imu.zof[i]=0;
}

void InitializeMPU6050(void)
{
	I2CInitialize();
	Delay(10);
	SetupMPU6050();
}

void ResetMPU6050(void)
{
	SetupMPU6050();
}

void ReadMPU6050Register(void)
{
	Delay(1); imu.GX=I2CReceiveDataWord(0x43);
	Delay(1); imu.GY=I2CReceiveDataWord(0x45);
	Delay(1); imu.GZ=I2CReceiveDataWord(0x47);
	Delay(1); imu.AX=I2CReceiveDataWord(0x3b);
	Delay(1); imu.AY=I2CReceiveDataWord(0x3d);
	Delay(1); imu.AZ=I2CReceiveDataWord(0x3f);
	Delay(1); imu.TMP=I2CReceiveDataWord(0x41);
}

#ifndef SDR1
#define SDR1(a)  (((a)+   0x0)>> 1)
#define SDR2(a)  (((a)+   0x1)>> 2)
#define SDR3(a)  (((a)+   0x3)>> 3)
#define SDR4(a)  (((a)+   0x7)>> 4)
#define SDR5(a)  (((a)+   0xf)>> 5)
#define SDR6(a)  (((a)+  0x1f)>> 6)
#define SDR7(a)  (((a)+  0x3f)>> 7)
#define SDR8(a)  (((a)+  0x7f)>> 8)
#define SDR9(a)  (((a)+  0xff)>> 9)
#define SDR10(a) (((a)+ 0x1ff)>>10)
#define SDR11(a) (((a)+ 0x3ff)>>11)
#define SDR12(a) (((a)+ 0x7ff)>>12)
#define SDR13(a) (((a)+ 0xfff)>>13)
#define SDR14(a) (((a)+0x1fff)>>14)
#define SDR15(a) (((a)+0x3fff)>>15)
#define SDR16(a) (((a)+0x7fff)>>16)
#endif


static void ProcessSensor(long gx,long gy,long gz,long ax,long ay,long az,int reset)
{
	long sub;
	if(reset)
	{
		imu.GyroAngleX=imu.GyroAngleY=0;
		imu.GyroAVX=imu.GyroAVY=0;
		imu.AccAngleX=imu.AccAngleY=0;
		imu.CGAngleX=imu.CGAngleY=0;
		imu.CGFltX=imu.CGFltY=0;
		return;
	}
	// y=(1-r)y+ru=y+r(u-y)=y+(u-y)>>s
	// y<<s=y<<s+(u<<s-y<<s)>>s, Y=Y+(u-Y>>s)
//#define FLT(y,u) y=y+(u-SDR1(y))
#define FLT(y,u) y=(u)<<1
	FLT(imu.GyroAVX,SDR4(gx));   
	FLT(imu.GyroAVY,SDR4(gy));   
	FLT(imu.AccAngleX,( (ay<<2)));
	FLT(imu.AccAngleY,(-(ax<<2)));
	imu.GyroAngleX+=SDR9(imu.GyroAVX*143); // SDR9 for 500Hz sampling
	imu.GyroAngleY+=SDR9(imu.GyroAVY*143); // SDR8 for 250Hz sampling
	sub=imu.AccAngleX-imu.GyroAngleX;
	imu.CGFltX+=SDR13(sub-imu.CGFltX);
	imu.CGAngleX=imu.GyroAngleX+imu.CGFltX;
	// compgyro
	sub=imu.AccAngleY-imu.GyroAngleY;
	imu.CGFltY+=SDR13(sub-imu.CGFltY);
	imu.CGAngleY=imu.GyroAngleY+imu.CGFltY;
}

// carib check: 
// set16 6 2 0x00ff;set16 6 3 0xaa10;set16 6 1 1;
// set16 6 2 0x00ff;set16 6 3 0x5510;set16 6 1 1;


//int ReadFIFO(int zofmode)
int ReadMPU6050(int zofmode)
{
	int fc,i,j;
	unsigned int r1,r2;
	
	SDelay();
    fc=I2CReceiveDataWord(0x72);
	fc=fc/2;
	if(fc>FIFObuffN) fc=FIFObuffN;
	imu.FIFOc=fc%(imu.FIFOdataN);
	fc-=imu.FIFOc;
	if(fc==0) return zofmode;

	// zero offset mode
	if((imu.zofc==0)&&(zofmode))
	{
		imu.zofc=256;
		for(i=0;i<imu.FIFOdataN;i++) imu.zof[i]=0;
		ProcessSensor(0,0,0,0,0,0, 1);
	}

	SDelay();
	// burst read from FIFO
	StartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMUWADDR);	IdleI2C1();
	MasterWriteI2C1(0x74);	IdleI2C1();
	RestartI2C1();	IdleI2C1();
	MasterWriteI2C1(IMURADDR);	IdleI2C1();
	for(i=0,j=0;i<fc;i++,j++)
	{
		r1=MasterReadI2C1();
		AckI2C1();	IdleI2C1();
		r2=MasterReadI2C1();
		if(i==fc-1) NotAckI2C1();
		else AckI2C1();
		if(j==imu.FIFOdataN) j=0;
		imu.FIFObuff[i]=(r1<<8)|r2;
		IdleI2C1();
	}
	StopI2C1();

	for(j=0;j<fc;j+=imu.FIFOdataN)
	{
		if(imu.zofc>256) { imu.zofc--; continue; }
		if(imu.zofc>0)
		{
			for(i=0;i<imu.FIFOdataN;i++)
                imu.zof[i]+=imu.FIFObuff[i+j];
			imu.zofc--;
			if(imu.zofc==0)
			{
				zofmode=0;
			}
		}
		else
		{
			imu.TMP=imu.FIFObuff[j+3];
			imu.GX=imu.FIFObuff[j+4];
			imu.GY=imu.FIFObuff[j+5];
			imu.GZ=imu.FIFObuff[j+6];
			imu.AX=imu.FIFObuff[j+0];
			imu.AY=imu.FIFObuff[j+1];
			imu.AZ=imu.FIFObuff[j+2];

			ProcessSensor(((long)imu.GX<<8)-imu.zof[4],((long)imu.GY<<8)-imu.zof[5],((long)imu.GZ<<8)-imu.zof[6],
							((long)imu.AX<<8)-imu.zof[0],((long)imu.AY<<8)-imu.zof[1],((long)imu.AZ<<8)-imu.zof[2], 0);
		}
	}
	return zofmode;
}

