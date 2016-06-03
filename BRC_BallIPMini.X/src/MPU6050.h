
// header for MPU6050.c
// which is tuned for dsPIC/PIC24 with I2C
// Customize is required for clock setting in I2CInitialize()
// and Delay(), SDelay() functions

// Initializer; this function is enough for rpeparation
extern void InitializeMPU6050(void);  

// periodic reading task of MPU6050; call this in the main control loop
// results will be stored into following structure 'imu'
// set zofmode 1 invoke offset calibration
extern int ReadMPU6050(int zofmode);

// MPU6050 initializer; called in InitializeMPU6050
extern void SetupMPU6050(void);
// Device reset function
extern void ResetMPU6050(void);


// MPU6050 I2C Addr 
// 1101000[r/w], same as IMU3000
#define IMUWADDR 0xd0
#define IMURADDR 0xd1

#define FIFObuffN 30

typedef struct _IMUDataStruct
{
	// FIFO reading buffer
	int FIFObuff[FIFObuffN];
	int FIFOc;
	// number of state variables
	int FIFOdataN;  
	// Raw data
	int GX,GY,GZ,AX,AY,AZ,TMP;
	// Process data
	long GyroAngleX,GyroAngleY;
	long GyroAVX,GyroAVY;
	long AccAngleX,AccAngleY;
	long CGAngleX,CGAngleY;
	long CGFltX,CGFltY;

	// zero offset
	int zofc;    // count timer
	long zof[10];  // offset
} IMUDataStruct;


extern IMUDataStruct imu;

#define IsIMUZeroDetect()  (imu.zofc>0)

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
