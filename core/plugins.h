//
// Based on PCSX-r code, modified for use in PCSX-HDBG
//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2021  "chekwob" <chek@wobbyworks.com>
//
// GNU GPL v3 or later
//

/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.           *
 ***************************************************************************/

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psxcommon.h"

typedef long (*GPUopen)(void (*)(void), char *, char *);
typedef long (*SPUopen)(void);
typedef long (*NETopen)(unsigned long *);
typedef long (*SIO1open)(unsigned long *);

#include "spu.h"

#include "psemu_plugin_defs.h"
#include "decode_xa.h"

int LoadPlugins();
void ReleasePlugins();
int OpenPlugins();
void ClosePlugins();

typedef unsigned long (* PSEgetLibType)(void);
typedef unsigned long (* PSEgetLibVersion)(void);
typedef char *(* PSEgetLibName)(void);

// GPU Functions
typedef long (* GPUinit)(void);
typedef long (* GPUshutdown)(void);
typedef long (* GPUclose)(void);
typedef void (* GPUwriteStatus)(uint32_t);
typedef void (* GPUwriteData)(uint32_t);
typedef void (* GPUwriteDataMem)(uint32_t *, int);
typedef uint32_t (* GPUreadStatus)(void);
typedef uint32_t (* GPUreadData)(void);
typedef void (* GPUreadDataMem)(uint32_t *, int);
typedef long (* GPUdmaChain)(uint32_t *,uint32_t);
typedef void (* GPUupdateLace)(void);
typedef long (* GPUconfigure)(void);
typedef long (* GPUtest)(void);
typedef void (* GPUabout)(void);
typedef void (* GPUmakeSnapshot)(void);
typedef void (* GPUkeypressed)(int);
typedef void (* GPUdisplayText)(char *);
typedef struct {
	uint32_t ulFreezeVersion;
	uint32_t ulStatus;
	uint32_t ulControl[256];
	unsigned char psxVRam[1024*512*2];
} GPUFreeze_t;
typedef long (* GPUfreeze)(uint32_t, GPUFreeze_t *);
typedef long (* GPUgetScreenPic)(unsigned char *);
typedef long (* GPUshowScreenPic)(unsigned char *);
typedef void (* GPUclearDynarec)(void (*callback)(void));
typedef void (* GPUhSync)(int);
typedef void (* GPUvBlank)(int);
typedef void (* GPUvisualVibration)(uint32_t, uint32_t);
typedef void (* GPUcursor)(int, int, int);
typedef void (* GPUaddVertex)(short,short,s64,s64,s64);
typedef void (* GPUsetSpeed)(float); // 1.0 = natural speed

// GPU function pointers
extern GPUupdateLace    GPU_updateLace;
extern GPUinit          GPU_init;
extern GPUshutdown      GPU_shutdown;
extern GPUconfigure     GPU_configure;
extern GPUtest          GPU_test;
extern GPUabout         GPU_about;
extern GPUopen          GPU_open;
extern GPUclose         GPU_close;
extern GPUreadStatus    GPU_readStatus;
extern GPUreadData      GPU_readData;
extern GPUreadDataMem   GPU_readDataMem;
extern GPUwriteStatus   GPU_writeStatus;
extern GPUwriteData     GPU_writeData;
extern GPUwriteDataMem  GPU_writeDataMem;
extern GPUdmaChain      GPU_dmaChain;
extern GPUkeypressed    GPU_keypressed;
extern GPUdisplayText   GPU_displayText;
extern GPUmakeSnapshot  GPU_makeSnapshot;
extern GPUfreeze        GPU_freeze;
extern GPUgetScreenPic  GPU_getScreenPic;
extern GPUshowScreenPic GPU_showScreenPic;
extern GPUclearDynarec  GPU_clearDynarec;
extern GPUhSync         GPU_hSync;
extern GPUvBlank        GPU_vBlank;
extern GPUvisualVibration GPU_visualVibration;
extern GPUcursor        GPU_cursor;
extern GPUaddVertex     GPU_addVertex;
extern GPUsetSpeed     GPU_setSpeed;

// CD-ROM Functions
typedef long (* CDRinit)(void);
typedef long (* CDRshutdown)(void);
typedef long (* CDRopen)(void);
typedef long (* CDRclose)(void);
typedef long (* CDRgetTN)(unsigned char *);
typedef long (* CDRgetTD)(unsigned char, unsigned char *);
typedef long (* CDRreadTrack)(unsigned char *);
typedef unsigned char* (* CDRgetBuffer)(void);
typedef unsigned char* (* CDRgetBufferSub)(void);
typedef long (* CDRconfigure)(void);
typedef long (* CDRtest)(void);
typedef void (* CDRabout)(void);
typedef long (* CDRplay)(unsigned char *);
typedef long (* CDRstop)(void);
typedef long (* CDRsetfilename)(char *);
struct CdrStat {
	uint32_t Type;
	uint32_t Status;
	unsigned char Time[3];
};
typedef long (* CDRgetStatus)(struct CdrStat *);
typedef char* (* CDRgetDriveLetter)(void);
struct SubQ {
	char res0[12];
	unsigned char ControlAndADR;
	unsigned char TrackNumber;
	unsigned char IndexNumber;
	unsigned char TrackRelativeAddress[3];
	unsigned char Filler;
	unsigned char AbsoluteAddress[3];
	unsigned char CRC[2];
	char res1[72];
};
typedef long (* CDRreadCDDA)(unsigned char, unsigned char, unsigned char, unsigned char *);
typedef long (* CDRgetTE)(unsigned char, unsigned char *, unsigned char *, unsigned char *);

// CD-ROM function pointers
extern CDRinit               CDR_init;
extern CDRshutdown           CDR_shutdown;
extern CDRopen               CDR_open;
extern CDRclose              CDR_close;
extern CDRtest               CDR_test;
extern CDRgetTN              CDR_getTN;
extern CDRgetTD              CDR_getTD;
extern CDRreadTrack          CDR_readTrack;
extern CDRgetBuffer          CDR_getBuffer;
extern CDRgetBufferSub       CDR_getBufferSub;
extern CDRplay               CDR_play;
extern CDRstop               CDR_stop;
extern CDRgetStatus          CDR_getStatus;
extern CDRgetDriveLetter     CDR_getDriveLetter;
extern CDRconfigure          CDR_configure;
extern CDRabout              CDR_about;
extern CDRsetfilename        CDR_setfilename;
extern CDRreadCDDA           CDR_readCDDA;
extern CDRgetTE              CDR_getTE;

// SPU Functions
typedef long (* SPUinit)(void);
typedef long (* SPUshutdown)(void);
typedef long (* SPUclose)(void);
typedef void (* SPUplaySample)(unsigned char);
typedef void (* SPUwriteRegister)(unsigned long, unsigned short);
typedef unsigned short (* SPUreadRegister)(unsigned long);
typedef void (* SPUwriteDMA)(unsigned short);
typedef unsigned short (* SPUreadDMA)(void);
typedef void (* SPUwriteDMAMem)(unsigned short *, int);
typedef void (* SPUreadDMAMem)(unsigned short *, int);
typedef void (* SPUplayADPCMchannel)(xa_decode_t *);
typedef void (* SPUregisterCallback)(void (*callback)(void));
typedef long (* SPUconfigure)(void);
typedef long (* SPUtest)(void);
typedef void (* SPUabout)(void);
typedef struct {
	unsigned char PluginName[8];
	uint32_t PluginVersion;
	uint32_t Size;
	unsigned char SPUPorts[0x200];
	unsigned char SPURam[0x80000];
	xa_decode_t xa;
	unsigned char *SPUInfo;
} SPUFreeze_t;
typedef long (* SPUfreeze)(uint32_t, SPUFreeze_t *);
typedef void (* SPUasync)(uint32_t);
typedef void (* SPUplayCDDAchannel)(short *, int);

// SPU function pointers
extern SPUconfigure        SPU_configure;
extern SPUabout            SPU_about;
extern SPUinit             SPU_init;
extern SPUshutdown         SPU_shutdown;
extern SPUtest             SPU_test;
extern SPUopen             SPU_open;
extern SPUclose            SPU_close;
extern SPUplaySample       SPU_playSample;
extern SPUwriteRegister    SPU_writeRegister;
extern SPUreadRegister     SPU_readRegister;
extern SPUwriteDMA         SPU_writeDMA;
extern SPUreadDMA          SPU_readDMA;
extern SPUwriteDMAMem      SPU_writeDMAMem;
extern SPUreadDMAMem       SPU_readDMAMem;
extern SPUplayADPCMchannel SPU_playADPCMchannel;
extern SPUfreeze           SPU_freeze;
extern SPUregisterCallback SPU_registerCallback;
extern SPUasync            SPU_async;
extern SPUplayCDDAchannel  SPU_playCDDAchannel;

// PAD Functions
typedef unsigned char (* PADstartPoll)(int);
typedef unsigned char (* PADpoll)(unsigned char);

// PAD function pointers
extern PADstartPoll        PAD1_startPoll;
extern PADpoll             PAD1_poll;
extern PADstartPoll        PAD2_startPoll;
extern PADpoll             PAD2_poll;

// NET Functions
typedef long (* NETinit)(void);
typedef long (* NETshutdown)(void);
typedef long (* NETclose)(void);
typedef long (* NETconfigure)(void);
typedef long (* NETtest)(void);
typedef void (* NETabout)(void);
typedef void (* NETpause)(void);
typedef void (* NETresume)(void);
typedef long (* NETqueryPlayer)(void);
typedef long (* NETsendData)(void *, int, int);
typedef long (* NETrecvData)(void *, int, int);
typedef long (* NETsendPadData)(void *, int);
typedef long (* NETrecvPadData)(void *, int);

typedef struct {
	char EmuName[32];
	char CdromID[9];	// ie. 'SCPH12345', no \0 trailing character
	char CdromLabel[11];
	void *psxMem;
	GPUshowScreenPic GPU_showScreenPic;
	GPUdisplayText GPU_displayText;
	char GPUpath[256];	// paths must be absolute
	char SPUpath[256];
	char CDRpath[256];
	char MCD1path[256];
	char MCD2path[256];
	char BIOSpath[256];	// 'HLE' for internal bios
	char Unused[1024];
} netInfo;

typedef long (* NETsetInfo)(netInfo *);
typedef long (* NETkeypressed)(int);

// NET function pointers
extern NETinit               NET_init;
extern NETshutdown           NET_shutdown;
extern NETopen               NET_open;
extern NETclose              NET_close;
extern NETtest               NET_test;
extern NETconfigure          NET_configure;
extern NETabout              NET_about;
extern NETpause              NET_pause;
extern NETresume             NET_resume;
extern NETqueryPlayer        NET_queryPlayer;
extern NETsendData           NET_sendData;
extern NETrecvData           NET_recvData;
extern NETsendPadData        NET_sendPadData;
extern NETrecvPadData        NET_recvPadData;
extern NETsetInfo            NET_setInfo;
extern NETkeypressed         NET_keypressed;

#ifdef ENABLE_SIO1API

// SIO1 Functions (link cable)
typedef long (* SIO1init)(void);
typedef long (* SIO1shutdown)(void);
typedef long (* SIO1close)(void);
typedef long (* SIO1configure)(void);
typedef long (* SIO1test)(void);
typedef void (* SIO1about)(void);
typedef void (* SIO1pause)(void);
typedef void (* SIO1resume)(void);
typedef long (* SIO1keypressed)(int);
typedef void (* SIO1writeData8)(u8);
typedef void (* SIO1writeData16)(u16);
typedef void (* SIO1writeData32)(u32);
typedef void (* SIO1writeStat16)(u16);
typedef void (* SIO1writeStat32)(u32);
typedef void (* SIO1writeMode16)(u16);
typedef void (* SIO1writeMode32)(u32);
typedef void (* SIO1writeCtrl16)(u16);
typedef void (* SIO1writeCtrl32)(u32);
typedef void (* SIO1writeBaud16)(u16);
typedef void (* SIO1writeBaud32)(u32);
typedef u8  (* SIO1readData8)(void);
typedef u16 (* SIO1readData16)(void);
typedef u32 (* SIO1readData32)(void);
typedef u16 (* SIO1readStat16)(void);
typedef u32 (* SIO1readStat32)(void);
typedef u16 (* SIO1readMode16)(void);
typedef u32 (* SIO1readMode32)(void);
typedef u16 (* SIO1readCtrl16)(void);
typedef u32 (* SIO1readCtrl32)(void);
typedef u16 (* SIO1readBaud16)(void);
typedef u32 (* SIO1readBaud32)(void);
typedef void (* SIO1update)(uint32_t);
typedef void (* SIO1registerCallback)(void (*callback)(void));

// SIO1 function pointers
extern SIO1init               SIO1_init;
extern SIO1shutdown           SIO1_shutdown;
extern SIO1open               SIO1_open;
extern SIO1close              SIO1_close;
extern SIO1test               SIO1_test;
extern SIO1configure          SIO1_configure;
extern SIO1about              SIO1_about;
extern SIO1pause              SIO1_pause;
extern SIO1resume             SIO1_resume;
extern SIO1keypressed         SIO1_keypressed;
extern SIO1writeData8         SIO1_writeData8;
extern SIO1writeData16        SIO1_writeData16;
extern SIO1writeData32        SIO1_writeData32;
extern SIO1writeStat16        SIO1_writeStat16;
extern SIO1writeStat32        SIO1_writeStat32;
extern SIO1writeMode16        SIO1_writeMode16;
extern SIO1writeMode32        SIO1_writeMode32;
extern SIO1writeCtrl16        SIO1_writeCtrl16;
extern SIO1writeCtrl32        SIO1_writeCtrl32;
extern SIO1writeBaud16        SIO1_writeBaud16;
extern SIO1writeBaud32        SIO1_writeBaud32;
extern SIO1readData8          SIO1_readData8;
extern SIO1readData16         SIO1_readData16;
extern SIO1readData32         SIO1_readData32;
extern SIO1readStat16         SIO1_readStat16;
extern SIO1readStat32         SIO1_readStat32;
extern SIO1readMode16         SIO1_readMode16;
extern SIO1readMode32         SIO1_readMode32;
extern SIO1readCtrl16         SIO1_readCtrl16;
extern SIO1readCtrl32         SIO1_readCtrl32;
extern SIO1readBaud16         SIO1_readBaud16;
extern SIO1readBaud32         SIO1_readBaud32;
extern SIO1update             SIO1_update;
extern SIO1registerCallback   SIO1_registerCallback;

#endif

void clearDynarec(void);

void SetIsoFile(const char *filename);
void SetExeFile(const char *filename);
void SetAppPath(const char *filename);
void SetLdrFile(const char *ldrfile );
const char *GetIsoFile(void);
const char *GetExeFile(void);
const char *GetAppPath(void);
const char *GetLdrFile(void);
boolean UsingIso(void);
void SetCdOpenCaseTime(s64 time);

#ifdef __cplusplus
}
#endif
#endif
