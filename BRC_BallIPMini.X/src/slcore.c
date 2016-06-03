/*   Serial Loop        By KUMAGAI Masaaki
 *      Rev. 030126
 *      Rev. 060929 ver 1.0 revised
 *      Rev. 061003 ver 1.1 revised for SL1.1
 *      Rev. 091119 ver 1.2 revised for dsPIC compati
 *
 *       normal Node : 
 *
 * Configurations:  see slnode.h
 *
 * number of reg files
 * #define REGFILEC 16
 */

#include "slnode.h"

#ifndef InvokeTransmit
extern void InvokeTransmit(SLWork *slw);
#endif
#ifndef EnableUARTInt
extern void EnableUARTInt(SLWork *slw);
#endif
#ifndef DisableUARTInt
extern void DisableUARTInt(SLWork *slw);
#endif

volatile unsigned int RegFileS[REGFILEC];
volatile unsigned long RegFileL[REGFILEC];

#ifdef USE_BULK
volatile unsigned int BulkBuffer[BULKBUFFSIZE];
#endif

SLWork _slwk;  // limited one channel

static void SLTXBClear(void)
{
	_slwk.twp = _slwk.trp = 0;
}

static int SLTXBLength(void)
{
	if(_slwk.twp >= _slwk.trp) return _slwk.twp-_slwk.trp;
	else return _slwk.twp-_slwk.trp+SLTXBLEN;
}

int SLTXBPutchar(char c)
{
	if(SLTXBLength()==SLTXBLEN-1) return -1;
	_slwk.txb[_slwk.twp]=c;
	_slwk.twp++;
	if(_slwk.twp>=SLTXBLEN) _slwk.twp=0;
	InvokeTransmit(&_slwk);
	return 0;
}

unsigned char burstwork[4];


#ifdef MASTER_RFBSIZE  /* master regfile size */
unsigned int NodeRegFileS[MASTER_RFBSIZE][REGFILEC];
unsigned long NodeRegFileL[MASTER_RFBSIZE][REGFILEC];
#endif

/* put packet on transmitting buffer */
void PutPacket(void)
{
	int i,pc;
	unsigned char *p;
	if((_slwk.rxc!=0)||(_slwk.pkc==0)) return; /* not rx idle (must wait) or no packet */
	if(SLTXBLength()>SLTXBLEN-_slwk.pkc)
	{
		/* not enough space */
		_slwk.pkc=0;
		return;
	}
	p=(unsigned char *)_slwk.pkb;
	pc=_slwk.pkc;
	for(i=0;i<pc;i++,p++)
	{
		_slwk.txb[_slwk.twp]= *p;
		_slwk.twp++;
		if(_slwk.twp>=SLTXBLEN) _slwk.twp=0;
	}
	_slwk.pkc=0;
	InvokeTransmit(&_slwk);
	return ;
}

/* Put packet on packet buffer, not immediately send */
int SendPacket(unsigned char *packet,int len)
{
	int i;
	unsigned char *s,*d;
	if(_slwk.pkc+len>SLPKBLEN) return 1;  /* not enough space */ 

	/* put packet */
	DisableUARTInt(&_slwk);
	s=packet;  d=_slwk.pkb+_slwk.pkc;
	for(i=len;i>0;i--,s++,d++)
		*d=*s;
	_slwk.pkc+=len;
	EnableUARTInt(&_slwk);

	/* here, if there are something to do, inttruped */
	DisableUARTInt(&_slwk);
	PutPacket();
	EnableUARTInt(&_slwk);
	return 0;
}


/* =============================================================*/
/* Packet Operation                                             */
/* =============================================================*/

/* packet building */
static unsigned char _SLPT[10];

/* =============================*/
/* SL1.1                        */
/* =============================*/
#ifdef USESL11

static unsigned char CalculateSL11Sum(unsigned char *d,int l)
{
  int i;
  unsigned char s=0,x=0;
  for(i=0;i<l;i++)
    {
      s+=d[i];
      x^=d[i];
    }
  return s^(x<<4);
}

/* Destination set */
#define SLDSetup(p,d) if(d&SLBroadcast) { p[0]=0x80|(_slwk.nid&0x3f); } \
                      else {p[0]=0x40|(d&0x3f); }
/* Source set */
#define SLSSetup(p) p[4]=_slwk.nid&0x3f;
#define SLPConPoint 3  /* Content point */
/* Make up header */
#define SLPMakeup(p,l) p[1]=(l); p[2]=~(l); p[l+3]=CalculateSL11Sum(p+3,l);
/* length of header/footer */
#define SLPHeadLen 4

#define SLPReqSRC (_slwk.rxb[SLPConPoint+1]&0x3f)
#define SLPRepSetDest _slwk.pkb[_slwk.pkc+0]=(0x40|SLPReqSRC);
#define SLPRepSetup(l) \
   _slwk.pkb[_slwk.pkc+1]=(l); _slwk.pkb[_slwk.pkc+2]=~(l); \
   _slwk.pkb[_slwk.pkc+3+l]=CalculateSL11Sum(_slwk.pkb+_slwk.pkc+3,l); \
   _slwk.pkc+=l+SLPHeadLen;

#else
/* =============================*/
/* SL1.0                        */
/* =============================*/

/* Destination set */
#define SLDSetup(p,d) if(d&SLBroadcast) { p[0]=0x80|(_slwk.nid&0x3f); } \
                      else {p[0]=0x40|(d&0x3f); }
/* Source set */
#define SLSSetup(p) p[3]=_slwk.nid&0x3f;
#define SLPConPoint 2  /* Content point */
/* Make up header */
#define SLPMakeup(p,l) p[1]=l;
/* length of header/footer */
#define SLPHeadLen 2

#define SLPReqSRC (_slwk.rxb[SLPConPoint+1]&0x3f)
#define SLPRepSetDest _slwk.pkb[_slwk.pkc+0]=(0x40|SLPReqSRC);
#define SLPRepSetup(l) _slwk.pkb[_slwk.pkc+1]=l; _slwk.pkc+=l+SLPHeadLen;

#endif

#define _LB(a) (a)&0xff

void SLNop(void)
{
  _SLPT[0]=_SLPT[1]=_SLPT[2]=_SLPT[3]=0;
  SendPacket(_SLPT,1);
}

void SLNop4(void)
{
  _SLPT[0]=_SLPT[1]=_SLPT[2]=_SLPT[3]=0;
  SendPacket(_SLPT,4);
}

void SLSet16(unsigned short dest,unsigned short reg,unsigned short val)
{
  SLDSetup(_SLPT,dest);  SLSSetup(_SLPT);  /* 0,3 */
  _SLPT[SLPConPoint+0]=0x20|(reg&0xf);
  _SLPT[SLPConPoint+2]=_LB(val);     _SLPT[SLPConPoint+3]=_LB(val>>8);  
  SLPMakeup(_SLPT,4);
  SendPacket(_SLPT,SLPHeadLen+4);
}

void SLSet32(unsigned short dest,unsigned short reg,unsigned long val)
{
  SLDSetup(_SLPT,dest);  SLSSetup(_SLPT);  /* 0,3 */
  _SLPT[SLPConPoint+0]=0x30|(reg&0xf);
  _SLPT[SLPConPoint+2]=_LB(val);     _SLPT[SLPConPoint+3]=_LB(val>>8);  
  _SLPT[SLPConPoint+4]=_LB(val>>16); _SLPT[SLPConPoint+5]=_LB(val>>24);
  SLPMakeup(_SLPT,6);
  SendPacket(_SLPT,SLPHeadLen+6);
}

void SLReply16(unsigned short dest,unsigned short reg,unsigned short val)
{
  SLDSetup(_SLPT,dest);  SLSSetup(_SLPT);  /* 0,3 */
  _SLPT[SLPConPoint+0]=0x60|(reg&0xf);
  _SLPT[SLPConPoint+2]=_LB(val);     _SLPT[SLPConPoint+3]=_LB(val>>8);
  SLPMakeup(_SLPT,4);
  SendPacket(_SLPT,SLPHeadLen+4);
}

void SLReply32(unsigned short dest,unsigned short reg,unsigned long val)
{
  SLDSetup(_SLPT,dest);  SLSSetup(_SLPT);  /* 0,3 */
  _SLPT[SLPConPoint+0]=0x70|(reg&0xf);
  _SLPT[SLPConPoint+2]=_LB(val);     _SLPT[SLPConPoint+3]=_LB(val>>8);  
  _SLPT[SLPConPoint+4]=_LB(val>>16); _SLPT[SLPConPoint+5]=_LB(val>>24);
  SLPMakeup(_SLPT,6);
  SendPacket(_SLPT,SLPHeadLen+6);
}

void SLGet16(unsigned short dest,unsigned short reg)
{
  SLDSetup(_SLPT,dest);  SLSSetup(_SLPT);  /* 0,3 */
  _SLPT[SLPConPoint+0]=0x40|(reg&0xf);
  SLPMakeup(_SLPT,2);
  SendPacket(_SLPT,SLPHeadLen+2);
}

void SLGet32(unsigned short dest,unsigned short reg)
{
  SLDSetup(_SLPT,dest);  SLSSetup(_SLPT);  /* 0,3 */
  _SLPT[SLPConPoint+0]=0x50|(reg&0xf);
  SLPMakeup(_SLPT,2);
  SendPacket(_SLPT,SLPHeadLen+2);
}

void SLReplyProfile(int dest,char *s)
{
    unsigned char work[100];
    int i;
    SLDSetup(work,dest);  SLSSetup(work);  /* 0,3 */
    work[SLPConPoint+0]=0x03;
    for(i=0;(i<80)&&(*s);i++,s++) // limited to 80 chars
        work[SLPConPoint+2+i]=((*s>=0x20)&&(*s<0x7f))?(*s):'.';
    SLPMakeup(work,2+i);
    SendPacket(work,SLPHeadLen+i+2);
}


/* =============================================================*/
/* /Packet Operation                                            */
/* =============================================================*/


/* Process received packet, whole packet was on rxb */
void SLPacketOperation(void)
{
  unsigned char *p;
  unsigned short int ws,wsh;
  unsigned long  int wd;
  int i,rid;
  if(_slwk.rxc==2) return;

  if(_slwk.rxb[SLPConPoint]<0x20)
  {
    switch(_slwk.rxb[SLPConPoint])
      {
      case 0x00:  /* device reset */
        return;
      case 0x02:  /* request Profile */
        SLPRepSetDest;
        _slwk.pkb[_slwk.pkc+SLPConPoint+0]=0x03;
        _slwk.pkb[_slwk.pkc+SLPConPoint+1]=_slwk.nid;
        for(i=SLPConPoint+2,p=(unsigned char *)(_slwk.profile);*p;i++,p++)
          _slwk.pkb[_slwk.pkc+i]=*p;
        SLPRepSetup(i-SLPConPoint);
        return;
      case 0x03:
        return;
      case 0x04:
 		_slwk.pkb[_slwk.pkc+SLPConPoint+0]=0x05;
 		_slwk.pkb[_slwk.pkc+SLPConPoint+1]=_slwk.nid;
		_slwk.nid=_slwk.rxb[SLPConPoint+2];
		SLPRepSetDest;
 		_slwk.pkb[_slwk.pkc+SLPConPoint+2]=_slwk.nid;
        SLPRepSetup(3);
		return;
      }
  } 

  /* special packet ================================ */  
#ifdef USE_MEMEXT
  if((_slwk.rxb[SLPConPoint]&0xf0)==0x80) /* memory extension command */
    {
      unsigned long addr;
      unsigned short val;
      int wf=(_slwk.rxb[SLPConPoint]&1);
      
      
      addr=((unsigned char *)_slwk.rxb)[SLPConPoint+2]|
           ((unsigned int)(_slwk.rxb[SLPConPoint+3])<<8)|
           ((unsigned long)(_slwk.rxb[SLPConPoint+4])<<16);
      val=wf?((unsigned int)(_slwk.rxb[SLPConPoint+6])<<8):0;    /* 16bit */
      val|=_slwk.rxb[SLPConPoint+5];
      switch(_slwk.rxb[SLPConPoint])
        {
        case 0x80: /* MRead */
        case 0x81:
          SLPRepSetDest;
          _slwk.pkb[_slwk.pkc+SLPConPoint+0]=0x82|wf;
          _slwk.pkb[_slwk.pkc+SLPConPoint+1]=_slwk.nid;
          _slwk.pkb[_slwk.pkc+SLPConPoint+2]=_slwk.rxb[SLPConPoint+2];
          _slwk.pkb[_slwk.pkc+SLPConPoint+3]=_slwk.rxb[SLPConPoint+3];
          _slwk.pkb[_slwk.pkc+SLPConPoint+4]=_slwk.rxb[SLPConPoint+4];
          val=wf?(*((unsigned short *)addr)):(*((unsigned char *)addr));
          _slwk.pkb[_slwk.pkc+SLPConPoint+5]=val&0xff;
          _slwk.pkb[_slwk.pkc+SLPConPoint+6]=val>>8;
          SLPRepSetup(6+wf);
          return;
        case 0x82: /* MReply : slave node will not receive it */
        case 0x83:
          return;
        case 0x84: /* MWrite */  
          *((unsigned char  *)addr) = val&0xff;  return;
        case 0x85:  
          *((unsigned short *)addr) = val;       return;
        case 0x86: /* MAnd */  
          *((unsigned char  *)addr) &= val&0xff;  return;
        case 0x87:  
          *((unsigned short *)addr) &= val;       return;
        case 0x88: /* MOr */  
          *((unsigned char  *)addr) |= val&0xff;  return;
        case 0x89:  
          *((unsigned short *)addr) |= val;       return;
        case 0x8a: /* MXor */  
          *((unsigned char  *)addr) ^= val&0xff;  return;
        case 0x8b:  
          *((unsigned short *)addr) ^= val;       return;
        }
    }
#endif        /* MEMEXT */
#ifdef USE_UIF_H8 /* UIF H8 is deleted in this version */
#endif
#ifdef USE_BULK /* Bulk transfer */
  if(_slwk.rxb[SLPConPoint]==0xa0) /* bulkset */
    {
      unsigned int *bp;
      int nd;
      rid=_slwk.rxb[SLPConPoint+2];  nd=_slwk.rxb[SLPConPoint+3];
      bp=(unsigned int *)BulkBuffer+rid;
      p=_slwk.rxb+SLPConPoint+4;
      for(i=0;i<nd;i++,bp++,p+=2)
        {
          *bp=((unsigned int)(p[0])<<8)|p[1];
        }
      return;
    }
  if(_slwk.rxb[SLPConPoint]==0xa1) /* bulkget */
    {
      unsigned int *bp;
      int nd;
      rid=_slwk.rxb[SLPConPoint+2];  nd=_slwk.rxb[SLPConPoint+3];
      bp=(unsigned int *)BulkBuffer+rid;
      SLPRepSetDest;
      p=(unsigned char *)_slwk.pkb+_slwk.pkc+SLPConPoint;
      p[0]=0xa2;
      p[1]=_slwk.nid;
      p[2]=rid;
      p[3]=nd;     p+=4;
      for(i=0;i<nd;i++,bp++,p+=2)
        {
          p[0]=(*bp)>>8;
          p[1]=(*bp)&0xff;
        }
      SLPRepSetup(4+nd*2);
      return;
    }
  /*if(_slwk.rxb[SLPConPoint]==0xa0) * bulkreply *
    { N/A
    }*/
#endif

  if((_slwk.rxb[SLPConPoint]<0x20)||(_slwk.rxb[SLPConPoint]>0x7f)) return;

/* register based operation ================================= */

  rid=_slwk.rxb[SLPConPoint]&0x0f;
  if(rid>=REGFILEC) rid=REGFILEC-1;
  switch(_slwk.rxb[SLPConPoint]&0xf0)
    {
    case 0x20: /* Set16 */
      RegFileS[rid]=((unsigned char *)(_slwk.rxb))[SLPConPoint+2]|((unsigned int)(_slwk.rxb[SLPConPoint+3])<<8);
      return;
    case 0x30: /* Set32 */
      ws =((unsigned char *)(_slwk.rxb))[SLPConPoint+2]|((unsigned int)(_slwk.rxb[SLPConPoint+3])<<8);
      wsh=((unsigned char *)(_slwk.rxb))[SLPConPoint+4]|((unsigned int)(_slwk.rxb[SLPConPoint+5])<<8);
      RegFileL[rid]=(((unsigned long)wsh)<<16)|ws;
      return;
    case 0x40: /* Get16 */
      SLPRepSetDest;
      _slwk.pkb[_slwk.pkc+SLPConPoint+0]=0x60|rid;
      _slwk.pkb[_slwk.pkc+SLPConPoint+1]=_slwk.nid;
      ws=RegFileS[rid];
      _slwk.pkb[_slwk.pkc+SLPConPoint+2]=_LB(ws);
      _slwk.pkb[_slwk.pkc+SLPConPoint+3]=_LB(ws>>8);
      SLPRepSetup(4);
      return;
    case 0x50: /* Get32 */
      SLPRepSetDest;
      _slwk.pkb[_slwk.pkc+SLPConPoint+0]=0x70|rid;
      _slwk.pkb[_slwk.pkc+SLPConPoint+1]=_slwk.nid;
      wd=RegFileL[rid];  ws=(unsigned short)wd; wsh=(wd>>16);
      _slwk.pkb[_slwk.pkc+SLPConPoint+2]=_LB(ws);
      _slwk.pkb[_slwk.pkc+SLPConPoint+3]=_LB(ws>>8);
      _slwk.pkb[_slwk.pkc+SLPConPoint+4]=_LB(wsh);
      _slwk.pkb[_slwk.pkc+SLPConPoint+5]=_LB(wsh>>8);
      SLPRepSetup(6);
      return;
#ifdef MASTER_RFBSIZE        /* as master, not completely implemented */
    case 0x60: /* Reply16 */
      nd=_slwk.rxb[SLPConPoint+1]&0x3f;
      /* nd=nd%MASTER_RFBSIZE; */
      if(nd>=MASTER_RFBSIZE) return;  /* �����O */
      NodeRegFileS[nd][rid]=((unsigned char *)(_slwk.rxb))[SLPConPoint+2]|
                            ((unsigned int)(_slwk.rxb[SLPConPoint+3])<<8);
      return;
    case 0x70: /* Reply32 */
      nd=_slwk.rxb[SLPConPoint+1]&0x3f;
      /* nd=nd%MASTER_RFBSIZE; */
      if(nd>=MASTER_RFBSIZE) return;  /* �����O */
      NodeRegFileL[nd][rid]=((unsigned char *)(_slwk.rxb))[SLPConPoint+2]|
                            ((unsigned int)(_slwk.rxb[SLPConPoint+3])<<8)|
                            ((unsigned long)(_slwk.rxb[SLPConPoint+4])<<16)|
                            ((unsigned long)(_slwk.rxb[SLPConPoint+5])<<24);
      return;
#endif
    }
  
  return;
}

/* Receive one byte from interrupt handle and build a packet */
void SLReceiveByte(unsigned char r)
{
  _slwk.rxb[_slwk.rxc]=r;
  if(_slwk.rxc==0)
    {
      _slwk.rs=SLRS_START;
      _slwk.tf=0;
      /* New packet start */
      /* nop, special, or destination */
      switch(r&0xc0)
        {
        case 0x00:
          if(r==0) 
           {
#ifdef ENABLE_FORWARD 
             SLTXBPutchar('\0');
#endif
             return; 
           }
          _slwk.rs=SLRS_SKIP; _slwk.tf=1; /* pass some unknow packet through */
          break;
        case 0xc0:
          _slwk.rs=SLRS_SKIP; _slwk.tf=0; break;
        case 0x40:
          if((r&0x3f)==_slwk.nid) { _slwk.rs=SLRS_RECV; _slwk.tf=0; } /* to me */
          else                    { _slwk.rs=SLRS_SKIP; _slwk.tf=1; } /* to others */
          break;
        case 0x80:
          if((r&0x3f)==_slwk.nid) { _slwk.rs=SLRS_SKIP; _slwk.tf=0; }  /* broadcast by me:stop */
          else                    { _slwk.rs=SLRS_RECV; _slwk.tf=1; }  /* broadcast by other */
          break;
        }
      _slwk.rxc++;   /* received 1byte */
#ifdef ENABLE_FORWARD
      if(_slwk.tf) SLTXBPutchar(r);
#endif
      return; 
    }
  _slwk.rxc++;
#ifdef ENABLE_FORWARD
  if(_slwk.tf) SLTXBPutchar(r);
#endif

#ifdef USESL11
#define SL11ML 40  /* length in maximum, assumed, in SL11 */
  if((_slwk.rxb[1]>SL11ML)||
     ((_slwk.rxc==3)&&(_slwk.rxb[1]!=((~(_slwk.rxb[2]))&0xff)))) /* error in packet length*/
     { /* cannot recover */
       _slwk.rxc=0;
       _slwk.pkb[_slwk.pkc]=_slwk.pkb[_slwk.pkc+1]=_slwk.pkb[_slwk.pkc+2]=0;
       _slwk.pkc+=3;  /* instead, send nop */
       PutPacket();
       return;
     }
#endif

  if(_slwk.rxc==_slwk.rxb[1]+SLPHeadLen) /* packet length completed */
    {
      if(_slwk.rs==SLRS_RECV) { SLPacketOperation(); } /* decoding packet */
      _slwk.rxc=0;
      PutPacket();   /* put packet after receive */
    }
  return;
}

int InitializeSerialLoop(int ch,int baudrate,
                         int nodeid,char *profile)
{
  int r;
  _slwk.profile=profile;
  _slwk.nid=nodeid;
  _slwk.ch=ch;

  SLTXBClear();
  _slwk.rxc=_slwk.pkc=0;
  r=SLInitializeUART(&_slwk, baudrate);
  if(r<0) return -1;
  EnableUARTInt();
  SLTXBPutchar('\0');
  return 0;
}



