//
// Based on PCSX-r code, modified for use in PCSX-HDBG
//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2020  "chekwob" <chek@wobbyworks.com>
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

/*
* Plugin library callback/access functions.
*/

#include "plugins.h"
#include "cdriso.h"

#include <hdbg_pad.h>

static char IsoFile[MAXPATHLEN] = "";
static char ExeFile[MAXPATHLEN] = "";
static char AppPath[MAXPATHLEN] = "";		//Application path(== pcsxr.exe directory)
static char LdrFile[MAXPATHLEN] = "";		//bin-load file
static s64 cdOpenCaseTime = 0;

GPUupdateLace         GPU_updateLace;
GPUinit               GPU_init;
GPUshutdown           GPU_shutdown;
GPUconfigure          GPU_configure;
GPUtest               GPU_test;
GPUabout              GPU_about;
GPUopen               GPU_open;
GPUclose              GPU_close;
GPUreadStatus         GPU_readStatus;
GPUreadData           GPU_readData;
GPUreadDataMem        GPU_readDataMem;
GPUwriteStatus        GPU_writeStatus;
GPUwriteData          GPU_writeData;
GPUwriteDataMem       GPU_writeDataMem;
GPUdmaChain           GPU_dmaChain;
GPUkeypressed         GPU_keypressed;
GPUdisplayText        GPU_displayText;
GPUmakeSnapshot       GPU_makeSnapshot;
GPUfreeze             GPU_freeze;
GPUgetScreenPic       GPU_getScreenPic;
GPUshowScreenPic      GPU_showScreenPic;
GPUclearDynarec       GPU_clearDynarec;
GPUhSync              GPU_hSync;
GPUvBlank             GPU_vBlank;
GPUvisualVibration    GPU_visualVibration;
GPUcursor             GPU_cursor;
GPUaddVertex          GPU_addVertex;
GPUsetSpeed           GPU_setSpeed;

CDRinit               CDR_init;
CDRshutdown           CDR_shutdown;
CDRopen               CDR_open;
CDRclose              CDR_close;
CDRtest               CDR_test;
CDRgetTN              CDR_getTN;
CDRgetTD              CDR_getTD;
CDRreadTrack          CDR_readTrack;
CDRgetBuffer          CDR_getBuffer;
CDRplay               CDR_play;
CDRstop               CDR_stop;
CDRgetStatus          CDR_getStatus;
CDRgetDriveLetter     CDR_getDriveLetter;
CDRgetBufferSub       CDR_getBufferSub;
CDRconfigure          CDR_configure;
CDRabout              CDR_about;
CDRsetfilename        CDR_setfilename;
CDRreadCDDA           CDR_readCDDA;
CDRgetTE              CDR_getTE;

SPUconfigure          SPU_configure;
SPUabout              SPU_about;
SPUinit               SPU_init;
SPUshutdown           SPU_shutdown;
SPUtest               SPU_test;
SPUopen               SPU_open;
SPUclose              SPU_close;
SPUplaySample         SPU_playSample;
SPUwriteRegister      SPU_writeRegister;
SPUreadRegister       SPU_readRegister;
SPUwriteDMA           SPU_writeDMA;
SPUreadDMA            SPU_readDMA;
SPUwriteDMAMem        SPU_writeDMAMem;
SPUreadDMAMem         SPU_readDMAMem;
SPUplayADPCMchannel   SPU_playADPCMchannel;
SPUfreeze             SPU_freeze;
SPUregisterCallback   SPU_registerCallback;
SPUasync              SPU_async;
SPUplayCDDAchannel    SPU_playCDDAchannel;

PADstartPoll          PAD1_startPoll;
PADpoll               PAD1_poll;

PADstartPoll          PAD2_startPoll;
PADpoll               PAD2_poll;

NETinit               NET_init;
NETshutdown           NET_shutdown;
NETopen               NET_open;
NETclose              NET_close;
NETtest               NET_test;
NETconfigure          NET_configure;
NETabout              NET_about;
NETpause              NET_pause;
NETresume             NET_resume;
NETqueryPlayer        NET_queryPlayer;
NETsendData           NET_sendData;
NETrecvData           NET_recvData;
NETsendPadData        NET_sendPadData;
NETrecvPadData        NET_recvPadData;
NETsetInfo            NET_setInfo;
NETkeypressed         NET_keypressed;

#ifdef ENABLE_SIO1API

SIO1init              SIO1_init;
SIO1shutdown          SIO1_shutdown;
SIO1open              SIO1_open;
SIO1close             SIO1_close;
SIO1test              SIO1_test;
SIO1configure         SIO1_configure;
SIO1about             SIO1_about;
SIO1pause             SIO1_pause;
SIO1resume            SIO1_resume;
SIO1keypressed        SIO1_keypressed;
SIO1writeData8        SIO1_writeData8;
SIO1writeData16       SIO1_writeData16;
SIO1writeData32       SIO1_writeData32;
SIO1writeStat16       SIO1_writeStat16;
SIO1writeStat32       SIO1_writeStat32;
SIO1writeMode16       SIO1_writeMode16;
SIO1writeMode32       SIO1_writeMode32;
SIO1writeCtrl16       SIO1_writeCtrl16;
SIO1writeCtrl32       SIO1_writeCtrl32;
SIO1writeBaud16       SIO1_writeBaud16;
SIO1writeBaud32       SIO1_writeBaud32;
SIO1readData8         SIO1_readData8;
SIO1readData16        SIO1_readData16;
SIO1readData32        SIO1_readData32;
SIO1readStat16        SIO1_readStat16;
SIO1readStat32        SIO1_readStat32;
SIO1readMode16        SIO1_readMode16;
SIO1readMode32        SIO1_readMode32;
SIO1readCtrl16        SIO1_readCtrl16;
SIO1readCtrl32        SIO1_readCtrl32;
SIO1readBaud16        SIO1_readBaud16;
SIO1readBaud32        SIO1_readBaud32;
SIO1update            SIO1_update;
SIO1registerCallback  SIO1_registerCallback;

#endif

static const char *err;

#define CheckErr(func) { \
	err = SysLibError(); \
	if (err != NULL) { SysMessage(_("Error loading %s: %s"), func, err); return -1; } \
}

#define LoadSym(dest, src, name, checkerr) { \
	dest = (src)SysLoadSym(drv, name); \
	if (checkerr) { CheckErr(name); } else SysLibError(); \
}

void *hGPUDriver = NULL;

void GPU__displayText(char *pText) {
	SysPrintf("%s\n", pText);
}

long GPU__configure(void) { return 0; }
long GPU__test(void) { return 0; }
void GPU__about(void) {}
void GPU__makeSnapshot(void) {}
void GPU__keypressed(int key) {}
long GPU__getScreenPic(unsigned char *pMem) { return -1; }
long GPU__showScreenPic(unsigned char *pMem) { return -1; }
void GPU__clearDynarec(void (*callback)(void)) {}
void GPU__hSync(int val) {}
void GPU__vBlank(int val) {}
void GPU__visualVibration(unsigned long iSmall, unsigned long iBig) {}
void GPU__cursor(int player, int x, int y) {}
void GPU__addVertex(short sx,short sy,s64 fx,s64 fy,s64 fz) {}
void GPU__setSpeed(float newSpeed) {}

#define LoadGpuSym1(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, TRUE);

#define LoadGpuSym0(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, FALSE); \
	if (GPU_##dest == NULL) GPU_##dest = (GPU##dest) GPU__##dest;

#define LoadGpuSymN(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, FALSE);

static int LoadGPUplugin(const char *GPUdll) {
	void *drv;

	hGPUDriver = SysLoadLibrary(GPUdll);
	if (hGPUDriver == NULL) {
		GPU_configure = NULL;
		SysMessage (_("Could not load GPU plugin %s!\n%s"), GPUdll, SysLibError());
		return -1;
	}
	drv = hGPUDriver;
	LoadGpuSym1(init, "GPUinit");
	LoadGpuSym1(shutdown, "GPUshutdown");
	LoadGpuSym1(open, "GPUopen");
	LoadGpuSym1(close, "GPUclose");
	LoadGpuSym1(readData, "GPUreadData");
	LoadGpuSym1(readDataMem, "GPUreadDataMem");
	LoadGpuSym1(readStatus, "GPUreadStatus");
	LoadGpuSym1(writeData, "GPUwriteData");
	LoadGpuSym1(writeDataMem, "GPUwriteDataMem");
	LoadGpuSym1(writeStatus, "GPUwriteStatus");
	LoadGpuSym1(dmaChain, "GPUdmaChain");
	LoadGpuSym1(updateLace, "GPUupdateLace");
	LoadGpuSym0(keypressed, "GPUkeypressed");
	LoadGpuSym0(displayText, "GPUdisplayText");
	LoadGpuSym0(makeSnapshot, "GPUmakeSnapshot");
	LoadGpuSym1(freeze, "GPUfreeze");
	LoadGpuSym0(getScreenPic, "GPUgetScreenPic");
	LoadGpuSym0(showScreenPic, "GPUshowScreenPic");
	LoadGpuSym0(clearDynarec, "GPUclearDynarec");
    LoadGpuSym0(hSync, "GPUhSync");
    LoadGpuSym0(vBlank, "GPUvBlank");
    LoadGpuSym0(visualVibration, "GPUvisualVibration");
    LoadGpuSym0(cursor, "GPUcursor");
	LoadGpuSym0(addVertex, "GPUaddVertex");
	LoadGpuSym0(setSpeed, "GPUsetSpeed");
	LoadGpuSym0(configure, "GPUconfigure");
	LoadGpuSym0(test, "GPUtest");
	LoadGpuSym0(about, "GPUabout");

	return 0;
}

void *hCDRDriver = NULL;

long CDR__play(unsigned char *sector) { return 0; }
long CDR__stop(void) { return 0; }

long CDR__getStatus(struct CdrStat *stat) {
	if (cdOpenCaseTime < 0 || cdOpenCaseTime > (s64)time(NULL))
		stat->Status = 0x10;
	else
		stat->Status = 0;

	return 0;
}

char* CDR__getDriveLetter(void) { return NULL; }
long CDR__configure(void) { return 0; }
long CDR__test(void) { return 0; }
void CDR__about(void) {}
long CDR__setfilename(char*filename) { return 0; }

#define LoadCdrSym1(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, TRUE);

#define LoadCdrSym0(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, FALSE); \
	if (CDR_##dest == NULL) CDR_##dest = (CDR##dest) CDR__##dest;

#define LoadCdrSymN(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, FALSE);

static int LoadCDRplugin(const char *CDRdll) {
	void *drv;

	if (CDRdll == NULL) {
		cdrIsoInit();
		return 0;
	}

	hCDRDriver = SysLoadLibrary(CDRdll);
	if (hCDRDriver == NULL) {
		CDR_configure = NULL;
		SysMessage (_("Could not load CD-ROM plugin %s!\n%s"), CDRdll, SysLibError());
		return -1;
	}
	drv = hCDRDriver;
	LoadCdrSym1(init, "CDRinit");
	LoadCdrSym1(shutdown, "CDRshutdown");
	LoadCdrSym1(open, "CDRopen");
	LoadCdrSym1(close, "CDRclose");
	LoadCdrSym1(getTN, "CDRgetTN");
	LoadCdrSym1(getTD, "CDRgetTD");
	LoadCdrSym1(readTrack, "CDRreadTrack");
	LoadCdrSym1(getBuffer, "CDRgetBuffer");
	LoadCdrSym1(getBufferSub, "CDRgetBufferSub");
	LoadCdrSym0(play, "CDRplay");
	LoadCdrSym0(stop, "CDRstop");
	LoadCdrSym0(getStatus, "CDRgetStatus");
	LoadCdrSym0(getDriveLetter, "CDRgetDriveLetter");
	LoadCdrSym0(configure, "CDRconfigure");
	LoadCdrSym0(test, "CDRtest");
	LoadCdrSym0(about, "CDRabout");
	LoadCdrSym0(setfilename, "CDRsetfilename");
	LoadCdrSymN(readCDDA, "CDRreadCDDA");
	LoadCdrSymN(getTE, "CDRgetTE");

	return 0;
}

void *hSPUDriver = NULL;

long SPU__configure(void) { return 0; }
void SPU__about(void) {}
long SPU__test(void) { return 0; }

#define LoadSpuSym1(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, TRUE);

#define LoadSpuSym0(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, FALSE); \
	if (SPU_##dest == NULL) SPU_##dest = (SPU##dest) SPU__##dest;

#define LoadSpuSymN(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, FALSE);

static int LoadSPUplugin(const char *SPUdll) {
	void *drv;

	hSPUDriver = SysLoadLibrary(SPUdll);
	if (hSPUDriver == NULL) {
		SPU_configure = NULL;
		SysMessage (_("Could not load SPU plugin %s!\n%s"), SPUdll, SysLibError());
		return -1;
	}
	drv = hSPUDriver;
	LoadSpuSym1(init, "SPUinit");
	LoadSpuSym1(shutdown, "SPUshutdown");
	LoadSpuSym1(open, "SPUopen");
	LoadSpuSym1(close, "SPUclose");
	LoadSpuSym0(configure, "SPUconfigure");
	LoadSpuSym0(about, "SPUabout");
	LoadSpuSym0(test, "SPUtest");
	LoadSpuSym1(writeRegister, "SPUwriteRegister");
	LoadSpuSym1(readRegister, "SPUreadRegister");
	LoadSpuSym1(writeDMA, "SPUwriteDMA");
	LoadSpuSym1(readDMA, "SPUreadDMA");
	LoadSpuSym1(writeDMAMem, "SPUwriteDMAMem");
	LoadSpuSym1(readDMAMem, "SPUreadDMAMem");
	LoadSpuSym1(playADPCMchannel, "SPUplayADPCMchannel");
	LoadSpuSym1(freeze, "SPUfreeze");
	LoadSpuSym1(registerCallback, "SPUregisterCallback");
	LoadSpuSymN(async, "SPUasync");
	LoadSpuSymN(playCDDAchannel, "SPUplayCDDAchannel");

	return 0;
}

static unsigned char buf[256];
unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
unsigned char analogpar[9] = { 0x00, 0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int bufcount, bufc;

PadDataS padd1, padd2;

unsigned char _PADstartPoll(PadDataS *pad) {
    bufc = 0;

    switch (pad->controllerType) {
        case PSE_PAD_TYPE_MOUSE:
            mousepar[3] = pad->buttonStatus & 0xff;
            mousepar[4] = pad->buttonStatus >> 8;
            mousepar[5] = pad->moveX;
            mousepar[6] = pad->moveY;

            memcpy(buf, mousepar, 7);
            bufcount = 6;
            break;
        case PSE_PAD_TYPE_NEGCON: // npc101/npc104(slph00001/slph00069)
            analogpar[1] = 0x23;
            analogpar[3] = pad->buttonStatus & 0xff;
            analogpar[4] = pad->buttonStatus >> 8;
            analogpar[5] = pad->rightJoyX;
            analogpar[6] = pad->rightJoyY;
            analogpar[7] = pad->leftJoyX;
            analogpar[8] = pad->leftJoyY;

            memcpy(buf, analogpar, 9);
            bufcount = 8;
            break;
        case PSE_PAD_TYPE_ANALOGPAD: // scph1150
            analogpar[1] = 0x73;
            analogpar[3] = pad->buttonStatus & 0xff;
            analogpar[4] = pad->buttonStatus >> 8;
            analogpar[5] = pad->rightJoyX;
            analogpar[6] = pad->rightJoyY;
            analogpar[7] = pad->leftJoyX;
            analogpar[8] = pad->leftJoyY;

            memcpy(buf, analogpar, 9);
            bufcount = 8;
            break;
        case PSE_PAD_TYPE_ANALOGJOY: // scph1110
            analogpar[1] = 0x53;
            analogpar[3] = pad->buttonStatus & 0xff;
            analogpar[4] = pad->buttonStatus >> 8;
            analogpar[5] = pad->rightJoyX;
            analogpar[6] = pad->rightJoyY;
            analogpar[7] = pad->leftJoyX;
            analogpar[8] = pad->leftJoyY;

            memcpy(buf, analogpar, 9);
            bufcount = 8;
            break;
        case PSE_PAD_TYPE_STANDARD:
        default:
            stdpar[3] = pad->buttonStatus & 0xff;
            stdpar[4] = pad->buttonStatus >> 8;

            memcpy(buf, stdpar, 5);
            bufcount = 4;
    }

    return buf[bufc++];
}

unsigned char _PADpoll(unsigned char value) {
    if (bufc > bufcount) return 0;
    return buf[bufc++];
}

unsigned char PAD1__startPoll(int pad) {
    PadDataS padd;

    memset(&padd, 0, sizeof(padd));
    padd.controllerType = PSE_PAD_TYPE_ANALOGPAD;
    padd.buttonStatus = pad_getbuttons();
    pad_getanalogs(
        &padd.leftJoyX,
        &padd.leftJoyY,
        &padd.rightJoyX,
        &padd.rightJoyY
    );

    return _PADstartPoll(&padd);
}

unsigned char PAD1__poll(unsigned char value) {
    return _PADpoll(value);
}

static int LoadPAD1plugin(const char *PAD1dll) {
    PAD1_startPoll = PAD1__startPoll;
    PAD1_poll = PAD1__poll;

	return 0;
}

unsigned char PAD2__startPoll(int pad) {
	PadDataS padd;

    memset(&padd, 0, sizeof(padd));

	return _PADstartPoll(&padd);
}

unsigned char PAD2__poll(unsigned char value) {
	return _PADpoll(value);
}

static int LoadPAD2plugin(const char *PAD2dll) {
    PAD2_startPoll = PAD2__startPoll;
    PAD2_poll = PAD2__poll;

	return 0;
}

void *hNETDriver = NULL;

void NET__setInfo(netInfo *info) {}
void NET__keypressed(int key) {}
long NET__configure(void) { return 0; }
long NET__test(void) { return 0; }
void NET__about(void) {}

#define LoadNetSym1(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, TRUE);

#define LoadNetSymN(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, FALSE);

#define LoadNetSym0(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, FALSE); \
	if (NET_##dest == NULL) NET_##dest = (NET##dest) NET__##dest;

static int LoadNETplugin(const char *NETdll) {
	void *drv;

	hNETDriver = SysLoadLibrary(NETdll);
	if (hNETDriver == NULL) {
		SysMessage (_("Could not load NetPlay plugin %s!\n%s"), NETdll, SysLibError());
		return -1;
	}
	drv = hNETDriver;
	LoadNetSym1(init, "NETinit");
	LoadNetSym1(shutdown, "NETshutdown");
	LoadNetSym1(open, "NETopen");
	LoadNetSym1(close, "NETclose");
	LoadNetSymN(sendData, "NETsendData");
	LoadNetSymN(recvData, "NETrecvData");
	LoadNetSym1(sendPadData, "NETsendPadData");
	LoadNetSym1(recvPadData, "NETrecvPadData");
	LoadNetSym1(queryPlayer, "NETqueryPlayer");
	LoadNetSym1(pause, "NETpause");
	LoadNetSym1(resume, "NETresume");
	LoadNetSym0(setInfo, "NETsetInfo");
	LoadNetSym0(keypressed, "NETkeypressed");
	LoadNetSym0(configure, "NETconfigure");
	LoadNetSym0(test, "NETtest");
	LoadNetSym0(about, "NETabout");

	return 0;
}

#ifdef ENABLE_SIO1API

void *hSIO1Driver = NULL;

long SIO1__init(void) { return 0; }
long SIO1__shutdown(void) { return 0; }
long SIO1__open(void) { return 0; }
long SIO1__close(void) { return 0; }
long SIO1__configure(void) { return 0; }
long SIO1__test(void) { return 0; }
void SIO1__about(void) {}
void SIO1__pause(void) {}
void SIO1__resume(void) {}
long SIO1__keypressed(int key) { return 0; }
void SIO1__writeData8(u8 val) {}
void SIO1__writeData16(u16 val) {}
void SIO1__writeData32(u32 val) {}
void SIO1__writeStat16(u16 val) {}
void SIO1__writeStat32(u32 val) {}
void SIO1__writeMode16(u16 val) {}
void SIO1__writeMode32(u32 val) {}
void SIO1__writeCtrl16(u16 val) {}
void SIO1__writeCtrl32(u32 val) {}
void SIO1__writeBaud16(u16 val) {}
void SIO1__writeBaud32(u32 val) {}
u8 SIO1__readData8(void) { return 0; }
u16 SIO1__readData16(void) { return 0; }
u32 SIO1__readData32(void) { return 0; }
u16 SIO1__readStat16(void) { return 0; }
u32 SIO1__readStat32(void) { return 0; }
u16 SIO1__readMode16(void) { return 0; }
u32 SIO1__readMode32(void) { return 0; }
u16 SIO1__readCtrl16(void) { return 0; }
u32 SIO1__readCtrl32(void) { return 0; }
u16 SIO1__readBaud16(void) { return 0; }
u32 SIO1__readBaud32(void) { return 0; }
void SIO1__update(uint32_t t) {};
void SIO1__registerCallback(void (*callback)(void)) {};

#define LoadSio1Sym1(dest, name) \
    LoadSym(SIO1_##dest, SIO1##dest, name, TRUE);

#define LoadSio1SymN(dest, name) \
    LoadSym(SIO1_##dest, SIO1##dest, name, FALSE);

#define LoadSio1Sym0(dest, name) \
    LoadSym(SIO1_##dest, SIO1##dest, name, FALSE); \
    if (SIO1_##dest == NULL) SIO1_##dest = (SIO1##dest) SIO1__##dest;

static int LoadSIO1plugin(const char *SIO1dll) {
    void *drv;

    hSIO1Driver = SysLoadLibrary(SIO1dll);
    if (hSIO1Driver == NULL) {
		SysMessage (_("Could not load SIO1 plugin %s!\n%s"), SIO1dll, SysLibError());
		return -1;
    }
    drv = hSIO1Driver;

    LoadSio1Sym0(init, "SIO1init");
    LoadSio1Sym0(shutdown, "SIO1shutdown");
    LoadSio1Sym0(open, "SIO1open");
    LoadSio1Sym0(close, "SIO1close");
    LoadSio1Sym0(pause, "SIO1pause");
    LoadSio1Sym0(resume, "SIO1resume");
    LoadSio1Sym0(keypressed, "SIO1keypressed");
    LoadSio1Sym0(configure, "SIO1configure");
    LoadSio1Sym0(test, "SIO1test");
    LoadSio1Sym0(about, "SIO1about");
    LoadSio1Sym0(writeData8, "SIO1writeData8");
    LoadSio1Sym0(writeData16, "SIO1writeData16");
    LoadSio1Sym0(writeData32, "SIO1writeData32");
    LoadSio1Sym0(writeStat16, "SIO1writeStat16");
    LoadSio1Sym0(writeStat32, "SIO1writeStat32");
    LoadSio1Sym0(writeMode16, "SIO1writeMode16");
    LoadSio1Sym0(writeMode32, "SIO1writeMode32");
    LoadSio1Sym0(writeCtrl16, "SIO1writeCtrl16");
    LoadSio1Sym0(writeCtrl32, "SIO1writeCtrl32");
    LoadSio1Sym0(writeBaud16, "SIO1writeBaud16");
    LoadSio1Sym0(writeBaud32, "SIO1writeBaud32");
	LoadSio1Sym0(readData8, "SIO1readData8");
    LoadSio1Sym0(readData16, "SIO1readData16");
    LoadSio1Sym0(readData32, "SIO1readData32");
    LoadSio1Sym0(readStat16, "SIO1readStat16");
    LoadSio1Sym0(readStat32, "SIO1readStat32");
    LoadSio1Sym0(readMode16, "SIO1readMode16");
    LoadSio1Sym0(readMode32, "SIO1readMode32");
    LoadSio1Sym0(readCtrl16, "SIO1readCtrl16");
    LoadSio1Sym0(readCtrl32, "SIO1readCtrl32");
    LoadSio1Sym0(readBaud16, "SIO1readBaud16");
    LoadSio1Sym0(readBaud32, "SIO1readBaud32");
	LoadSio1Sym0(update, "SIO1update");
    LoadSio1Sym0(registerCallback, "SIO1registerCallback");

    return 0;
}

#endif

void clearDynarec(void) {
	psxCpu->Reset();
}

int LoadPlugins() {
	long ret;

	ReleasePlugins();

	if (UsingIso()) {
		LoadCDRplugin(NULL);
	} else {
		if (LoadCDRplugin(Config.Cdr) == -1) return -1;
	}

	if (LoadGPUplugin(Config.Gpu) == -1) return -1;

	if (LoadSPUplugin(Config.Spu) == -1) return -1;

	if (LoadPAD1plugin(Config.Pad1) == -1) return -1;

	if (LoadPAD2plugin(Config.Pad2) == -1) return -1;

	if (strcmp("Disabled", Config.Net) == 0 || strcmp("", Config.Net) == 0)
		Config.UseNet = FALSE;
	else {
		Config.UseNet = TRUE;
		if (LoadNETplugin(Config.Net) == -1) Config.UseNet = FALSE;
	}

#ifdef ENABLE_SIO1API
	if (LoadSIO1plugin(Config.Sio1) == -1) return -1;
#endif

	ret = CDR_init();
	if (ret < 0) { SysMessage (_("Error initializing CD-ROM plugin: %d"), ret); return -1; }
	ret = GPU_init();
	if (ret < 0) { SysMessage (_("Error initializing GPU plugin: %d"), ret); return -1; }
	ret = SPU_init();
	if (ret < 0) { SysMessage (_("Error initializing SPU plugin: %d"), ret); return -1; }

	if (Config.UseNet) {
		ret = NET_init();
		if (ret < 0) { SysMessage (_("Error initializing NetPlay plugin: %d"), ret); return -1; }
	}

#ifdef ENABLE_SIO1API
	ret = SIO1_init();
	if (ret < 0) { SysMessage (_("Error initializing SIO1 plugin: %d"), ret); return -1; }
#endif

	SysPrintf("%s", _("Plugins loaded.\n"));
	return 0;
}

void ReleasePlugins() {
	if (Config.UseNet) {
		long ret = NET_close();
		if (ret < 0) Config.UseNet = FALSE;
	}
	NetOpened = FALSE;

	if (hCDRDriver != NULL || cdrIsoActive()) CDR_shutdown();
	if (hGPUDriver != NULL) GPU_shutdown();
	if (hSPUDriver != NULL) SPU_shutdown();

	if (Config.UseNet && hNETDriver != NULL) NET_shutdown();

	if (hCDRDriver != NULL) SysCloseLibrary(hCDRDriver); hCDRDriver = NULL;
	if (hGPUDriver != NULL) SysCloseLibrary(hGPUDriver); hGPUDriver = NULL;
	if (hSPUDriver != NULL) SysCloseLibrary(hSPUDriver); hSPUDriver = NULL;

	if (Config.UseNet && hNETDriver != NULL) {
		SysCloseLibrary(hNETDriver); hNETDriver = NULL;
	}

#ifdef ENABLE_SIO1API
	if (hSIO1Driver != NULL) {
		SIO1_shutdown();
		SysCloseLibrary(hSIO1Driver);
		hSIO1Driver = NULL;
	}
#endif
}

void SetIsoFile(const char *filename) {
	if (filename == NULL) {
		IsoFile[0] = '\0';
		return;
	}
	strncpy(IsoFile, filename, MAXPATHLEN);
}

void SetExeFile(const char *filename) {
	if (filename == NULL) {
		ExeFile[0] = '\0';
		return;
	}
	strncpy(ExeFile, filename, MAXPATHLEN);
}

// Set pcsxr.exe directory. This is not contain filename(and ext)).
void SetAppPath(const char *apppath ) {
	if (apppath == NULL) {
		AppPath[0] = '\0';
		return;
	}
	strncpy(AppPath, apppath, MAXPATHLEN);
}

void SetLdrFile(const char *ldrfile ) {
	if (ldrfile == NULL) {
		LdrFile[0] = '\0';
		return;
	}
	strncpy(LdrFile, ldrfile, MAXPATHLEN);
}

const char *GetIsoFile(void) {
	return IsoFile;
}

const char *GetExeFile(void) {
	return ExeFile;
}

const char *GetAppPath(void) {
	return AppPath;
}

const char *GetLdrFile(void) {
	return LdrFile;
}

boolean UsingIso(void) {
	return (IsoFile[0] != '\0' || Config.Cdr[0] == '\0');
}

void SetCdOpenCaseTime(s64 time) {
	cdOpenCaseTime = time;
}
