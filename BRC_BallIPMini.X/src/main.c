#include "usb.h"
#include "usb_host_hid.h"

#include "brc.h"
#include "slnode.h"
#include "app_host_hid_joypad.h"
#include "MPU6050.h"
#include "FutabaServo.h"

void BRC_SetCWCCW(int ch,bool ccw)
{
    switch(ch)
    {
        case 0: LAT_CWCCW1=ccw; break;
        case 1: LAT_CWCCW2=ccw; break;
        case 2: LAT_CWCCW3=ccw; break;
    }
}

int DeadZone(int v,int dz)
{
    if(v> dz) return v-dz;
    if(v<-dz) return v+dz;
    return 0;
}

int main(void)
{   
    int i;

    BRC_InitializeLEDSW();
    BRC_LEDRed(1);
    BRC_LEDGreen(0);
    
    BRC_SetupClockPLL();
    BRC_SetupUSBClock();
    BRC_InitializeSysClock(10000l);
    BRC_IORemap();

    BRC_InitializeMotorPulse();
    // MotorPulse RP bind
    StartIORemap();
    RPOUT_CLK1=BRC_OCxRPO_MPCH(0);
    RPOUT_CLK2=BRC_OCxRPO_MPCH(1);
    RPOUT_CLK3=BRC_OCxRPO_MPCH(2);
    //RPOUT_CLK4=BRC_OCxRPO_MPCH(3);
    //RPOUT_CLK5=BRC_OCxRPO_MPCH(4);
    //RPOUT_CLK6=BRC_OCxRPO_MPCH(5);
    //RPOUT_DE21=BRC_OCxRPO_MPCH(1);
    //RPOUT_DE22=BRC_OCxRPO_MPCH(2);
    EndIORemap();

    // Serial Port Binding
#if 0
    StartIORemap();
  	RPINR19bits.U2RXR=RPIN_DE22;   // UART 2 RX <- RP_DE22
	RPOUT_DE21=3;      // RP_DE21 <- UART 2 TX
    EndIORemap();
    InitializeSerial(SIB115200);
#endif
    InitializeFutabaServo(SIB115200);
    
    TRIS_ENABLE12=TRIS_OUT;    TRIS_DCY12=TRIS_OUT;
    TRIS_M12=TRIS_OUT;         TRIS_TQ12=TRIS_OUT;
    LAT_ENABLE12=0;  LAT_DCY12=0;  LAT_M12=0;  LAT_TQ12=0;
    TRIS_CWCCW1=TRIS_OUT; TRIS_CWCCW2=TRIS_OUT; TRIS_CWCCW3=TRIS_OUT;
    LAT_CWCCW1=0; LAT_CWCCW2=0; LAT_CWCCW3=0;
    
    BRC_SetMotorSpeed(0,0);
    BRC_SetMotorSpeed(1,0);
    BRC_SetMotorSpeed(2,0);
    
    BRC_InitializeLEDSwitchBoard();
    
    InitializeMPU6050();
    ReadMPU6050(1);
    
#define Count RegFileL[0]
#define ControlCount RegFileL[1]
    Count=0;
    ControlCount=0;
    
    InitializeSerialLoop(1,SLB115200,4,"BRC_USBtest " __DATE__ " " __TIME__);
	SLReply32(63,1,0x12345678);
    SLReplyProfile(63,"start up");
    for(i=0;i<16;i++)
    {
        RegFileL[i]=0; RegFileS[i]=0;
    }

    // USB_JOYPAD ; do not change or use carefully
    // Initializer for USB interface and JoyPad handler
    USBHostInit(0);
    APP_HostHIDJoyPadInitialize();
    // /USB_JOYPAD

    // ANSEL: digital config (0)
    // ANSELA=0x0000; 
    ANSELB=0x0000; ANSELC=0x0000;
    ANSELD=0x0000; ANSELE=0x0000;
    // ANSELF=0x0000;
    ANSELG=0x0000;
    //TRIS_CCN10=TRIS_OUT; TRIS_CCN11=TRIS_OUT;
    //TRIS_CCN12=TRIS_OUT; TRIS_CCN13=TRIS_OUT;
    //TRIS_CCN14=TRIS_OUT; TRIS_CCN15=TRIS_OUT;
    //TRIS_CCN16=TRIS_OUT; TRIS_CCN17=TRIS_OUT;
    
    long clockstep=20;
    long nextclock=BRC_SysClock+clockstep;
    // state variables
    enum { SVCM_Position, SVCM_Velocity, SVCM_Lean };
    struct _SV 
    {
        long theta, thetav, pos, vel;
        long theta_ref, thetav_ref, pos_ref, vel_ref, pos_ref_0;
        long theta_0;
        long acc;
        int cm;  // controlmode 0: pos 1: vel 2: agl ; SVCM_*
        int gm[4];  // gainmask 1,0,-1
    } SV[2];
#define SVX (SV[0])
#define SVY (SV[1])
    SVX.pos=0; SVY.pos=0;
    SVX.cm=99; SVY.cm=99;
    SVX.theta_0=SVY.theta_0=0;
    for(i=0;i<4;i++) { SVX.gm[i]=SVY.gm[i]=1; }
    int YawRate=0;
    
    RegFileL[4]= 400;
    RegFileL[5]=3500;
    RegFileL[6]= 500;
    RegFileL[7]=5000;
    
    
    FSServoOnOff(1,1);
    
    while(1)
    {
        RegFileL[2]=nextclock-BRC_SysClock;
        while(nextclock>BRC_SysClock)
        {
            // USB_JOYPAD ; do not change or use carefully
            // These three functions are required for state machine tasks
            // of USB handlers; execute while main tasks are idel
            USBHostTasks();
            USBHostHIDTasks();
            APP_HostHIDJoyPadTasks();       
            // /USB_JOYPAD
        }
        Count++; nextclock+=clockstep;

        // main control

        // lean angle sensing
        // if zero detect commanded
        if((((padState.button&0x300)==0x300)||(BRC_SWState()))&&(!IsIMUZeroDetect()))
        {
            ReadMPU6050(1);  // zero detect mode
            SVX.theta_0=SVY.theta_0=0;
        }
        else
            ReadMPU6050(0);  // normal IMU function
        if(IsIMUZeroDetect())
            BRC_LEDGreen(1);
        else
            BRC_LEDGreen(0);

        unsigned long lastcc=ControlCount;
        if(((padState.button&0x1)==0x1)&&(!IsIMUZeroDetect()))
        {
            if(ControlCount<1000)
               ControlCount=1000;
        }
        if((((padState.button&0x5)==0x5)||(BRC_LEDSwitchBoard_GetPush()))&&(!IsIMUZeroDetect()))
        {
            ControlCount=10000000l;
        }
        if((padState.button&0xa)==0xa)
        {
            ControlCount=0;
        }
        
        if((lastcc==0)&&(ControlCount>0))
        {  // Control started now
            LAT_ENABLE12=1;
            SVX.theta_ref=0;  SVX.pos_ref_0=SVX.pos_ref=SVX.pos;
            SVY.theta_ref=0;  SVY.pos_ref_0=SVY.pos_ref=SVY.pos;
            SVX.vel=0;
            SVY.vel=0;
            SVX.thetav_ref=0; SVX.vel_ref=0;
            SVY.thetav_ref=0; SVY.vel_ref=0;
            BRC_LEDRed(1);
        }
        if(ControlCount>0) ControlCount--;
        if((lastcc>0)&&(ControlCount==0))
        {  // Control stopped now
            LAT_ENABLE12=0;
            BRC_LEDRed(0);
        }
        
        SVX.theta = imu.CGAngleY - SVX.theta_0;  // lean to x axis dir
        SVX.thetav= imu.GyroAVY;                 // angular vel of above 
        SVY.theta =-imu.CGAngleX - SVY.theta_0;  // lean to y axis dir
        SVY.thetav=-imu.GyroAVX;                 // angular vel of above 
        if(ControlCount>0)
        {  // main control
            for(i=0;i<2;i++)
            {
                SV[i].acc=SDR8(
                        SDR12((long)(RegFileL[4])*(SV[i].gm[0])*(SV[i].theta -SV[i].theta_ref))+
                        SDR12((long)(RegFileL[5])*(SV[i].gm[1])*(SV[i].thetav-SV[i].thetav_ref))+
                        SDR8( (long)(RegFileL[6])*(SV[i].gm[2])*(SV[i].pos   -SV[i].pos_ref))+
                        SDR8( (long)(RegFileL[7])*(SV[i].gm[3])*(SV[i].vel   -SV[i].vel_ref))
                       );
                SV[i].vel+=SDR4(SV[i].acc);
                SV[i].pos+=SDR4(SV[i].vel);
            }
            if((SVX.vel>30000)||(SVX.vel<-30000)) ControlCount=0; // force stop
            if((SVY.vel>30000)||(SVY.vel<-30000)) ControlCount=0;
            int ps0=SVX.vel;
            int ps1=SVY.vel;
            int ps2=YawRate;
            int ms0=  ps0    +                         +ps2;
            int ms1=-(ps0>>1)+(int)(((long)ps1*222)>>8)+ps2;
            int ms2=-(ps0>>1)-(int)(((long)ps1*222)>>8)+ps2;
            BRC_SetMotorSpeed(0,ms0);
            BRC_SetMotorSpeed(1,ms1);
            BRC_SetMotorSpeed(2,ms2);

            // lean angle equilibrium point
            if((padState.button&0x100)==0x100)            
            for(i=0;i<2;i++)
            {
                SV[i].theta_0-=SDR4(SV[i].pos-SV[i].pos_ref);
            }
        }
        else
        {
            SVX.vel=0;
            SVY.vel=0;
            BRC_SetMotorSpeed(0,0);
            BRC_SetMotorSpeed(1,0);
            BRC_SetMotorSpeed(2,0);
            LAT_ENABLE12=0;
            BRC_LEDRed(0);
        }
 
        BRC_LEDSwitchBoard_CycleTask();


        // USB_JOYPAD ; do not change or use carefully
        // request the JoyPad current stick/button state
        // results will be stored into padState (global) structure
        // in subsequent tasks in USB handlers
        if((Count&0xf)==0)
            APP_HostHIDInputRequest();
        // /USB_JOYPAD

        
        //BRC_LEDRed((count&1)?1:0);
#if 1
        //BRC_LEDSwitchBoard_SetLEDRaw(0x55aa55aal);
        //BRC_LEDSwitchBoard_SetHex(0x0123);
        //BRC_LEDSwitchBoard_SetHex(BRC_LEDSwitchBoard_GetSwitch());
        //BRC_LEDSwitchBoard_SetHex(Count);
        //BRC_LEDSwitchBoard_SetHex((int)(imu.CGAngleX>>8)+0x8000);
        //BRC_LEDSwitchBoard_SetHex(padState.axis[0]);
        //BRC_LEDRed(BRC_SWState());
        BRC_LEDRed(BRC_LEDSwitchBoard_GetPush());
        //BRC_LEDSwitchBoard_SetLEDRaw(RegFileL[4]);
        //BRC_LEDSwitchBoard_SetLED(RegFileL[4]);
        BRC_LEDSwitchBoard_SetLEDBits(0,IsIMUZeroDetect());
        BRC_LEDSwitchBoard_SetLEDBits(1,(ControlCount>0)?1:0);
//        BRC_LEDSwitchBoard_SetHex((int)(imu.CGAngleX>>8));
//      BRC_LEDSwitchBoard_SetSIntZS((int)(imu.CGAngleX>>8),3);
#define IntAngleToDeg(a)      ((((a)>>12)*358)>>9)
#define IntAngleVelToDegps(a) ((((a)>>10)*625)>>6)
#define IntPosToMm(a)         ((((a)>>5)*267)>>8)
#define IntVelToMmps(a)       (((a)*209)>>11)
        switch((BRC_LEDSwitchBoard_GetSwitch()>>12)&0xf)
        {
            case 0: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleToDeg(SVX.theta),3); BRC_LEDSwitchBoard_SetDP(2); break;
            case 1: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleToDeg(SVY.theta),3); BRC_LEDSwitchBoard_SetDP(2); break;
            case 2: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleVelToDegps(SVX.thetav),2);  BRC_LEDSwitchBoard_SetDP(1); break;
            case 3: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleVelToDegps(SVY.thetav),2);  BRC_LEDSwitchBoard_SetDP(1); break;
            case 4: BRC_LEDSwitchBoard_SetSIntZS((int)IntPosToMm(SVX.pos),2);         BRC_LEDSwitchBoard_SetDP(1); break;
            case 5: BRC_LEDSwitchBoard_SetSIntZS((int)IntPosToMm(SVY.pos),2);         BRC_LEDSwitchBoard_SetDP(1); break;
            case 6: BRC_LEDSwitchBoard_SetSIntZS((int)IntVelToMmps(SVX.vel),1);       BRC_LEDSwitchBoard_SetDP(0); break;
            case 7: BRC_LEDSwitchBoard_SetSIntZS((int)IntVelToMmps(SVY.vel),1);       BRC_LEDSwitchBoard_SetDP(0); break;
            case 8: BRC_LEDSwitchBoard_SetUInt(Count);              break;
            case 9: BRC_LEDSwitchBoard_SetSInt(RegFileL[2]&0xffff); break;
            case 10: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleToDeg(imu.CGAngleX),3); BRC_LEDSwitchBoard_SetDP(2); break;
            case 11: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleToDeg(imu.CGAngleY),3); BRC_LEDSwitchBoard_SetDP(2); break;
            case 12: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleVelToDegps(imu.GyroAVX),2);  BRC_LEDSwitchBoard_SetDP(1); break;
            case 13: BRC_LEDSwitchBoard_SetSIntZS((int)IntAngleVelToDegps(imu.GyroAVY),2);  BRC_LEDSwitchBoard_SetDP(1); break;
        }
#endif
        for(i=0;i<2;i++)
        {
            switch(SV[i].cm)
            {
                case SVCM_Position: BRC_LEDSwitchBoard_SetLEDBits(2+i,0); break;
                case SVCM_Velocity: BRC_LEDSwitchBoard_SetLEDBits(2+i,1); break;
                case SVCM_Lean: BRC_LEDSwitchBoard_SetLEDBits(2+i,(Count&0x10)?1:0); break;
            }
        }
        
      
        
        if(padState.updated)
        {
            int ps[3];
            int sh=2;
            ps[0]=DeadZone((int)((padState.axis[0])-0x8000)>>1,0x500);
            ps[1]=DeadZone((int)((padState.axis[1])-0x8000)>>1,0x500);
            ps[2]=DeadZone((int)((padState.axis[2])-0x8000)>>1,0x500);
            padState.updated=0;
            //SLReply16(63,0,padState.axis[0]);
            //SLReply16(63,1,(int)((((int)(padState.axis[0]))-0x8000))>>5);
            //SLReply16(63,4,padState.button);
            if(padState.button&0x0008) sh=0;
            YawRate=ps[2]>>(3+sh);
            // controlmode transition
            for(i=0;i<2;i++)
            {
                int ocm=SV[i].cm;
                SV[i].cm=SVCM_Position;
                if(padState.button&(0x010<<(i*2))) SV[i].cm=SVCM_Velocity;
                if(padState.button&(0x020<<(i*2))) SV[i].cm=SVCM_Lean;

                if(SV[i].cm!=ocm)  // mode_changed
                {
                    switch(SV[i].cm)
                    {
                        case SVCM_Position:
                            SV[i].gm[0]=1; SV[i].gm[1]=1; SV[i].gm[2]=1; SV[i].gm[3]=1;  // gain activation
                            SV[i].pos_ref_0=SV[i].pos_ref=SV[i].pos;
                            SV[i].vel_ref=0;  SV[i].theta_ref=0;
                            break;
                        case SVCM_Velocity:
                           SV[i].gm[0]=1; SV[i].gm[1]=1; SV[i].gm[2]=0; SV[i].gm[3]=1;  // gain activation
                            SV[i].vel_ref=0;  SV[i].theta_ref=0;
                            break;
                        case SVCM_Lean:
                            SV[i].gm[0]=1; SV[i].gm[1]=1; SV[i].gm[2]=0; SV[i].gm[3]=0;  // gain activation
                            SV[i].theta_ref=0;
                            break;
                    }
                }
                
                switch(SV[i].cm)
                {
                    case SVCM_Position:
                        SV[i].pos_ref=SV[i].pos_ref_0+((long)(ps[i])<<(4-sh));
                        break;
                    case SVCM_Velocity:
                        SV[i].vel_ref=ps[i]>>(sh);
                        break;
                    case SVCM_Lean:
                        SV[i].theta_ref=(long)(ps[i])<<(8-sh);
                        break;
                }


            }
            
        }
// servo
#if 0
        if((Count&0xf)==0)
        {
            if((Count&0xff)==0)
                   SLReply16(63,3,SerialGetRxCount());
            unsigned char rb[4];
            int id,addr,r,led=0;
            for(i=0;i<2;i++)
            {
                r=FSDecodeResponse(&id,&addr,4,rb);
                if(r<0)
                    BRC_LEDSwitchBoard_SetHex(0xee00+(-r));
                else
                {
                    unsigned int v=(((unsigned int)(rb[1]))<<8)|rb[0];
                    if(id==1) led=(led&~0x00ff)|(v>>1);
                    if(id==2) led=(led&~0xff00)|(v>>1<<8);
                    BRC_LEDSwitchBoard_SetHex(led);
                }
            }

            FSClearResponse();
            FSSendMemoryRequest(1,48,2);
            FSSendMemoryRequest(2,48,2);
        }
        FSCheckTimeout();
#endif
        if((Count&0x1f)==0)
        {
            SLReply16(63,8,(int)(imu.CGAngleX>>8));
            SLReply16(63,9,(int)(imu.CGAngleY>>8));
            //SLReply16(63,10,SerialGetTxCount());
            //SLReply16(63,11,SerialGetRxCount());
        }
   }
    
}


// gain setting
// set32 4 4 400; set32 4 5 3500;set32 4 6 500;set32 4 7 5000