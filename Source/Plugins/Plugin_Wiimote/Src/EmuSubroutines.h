// Copyright (C) 2003-2008 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/


//////////////////////////////////////////////////////////////////////////////////////////
// Include
// ���������
#ifndef _EMU_SUBFUNCTIONS_
#define _EMU_SUBFUNCTIONS_

#include "pluginspecs_wiimote.h"

#include <vector>
#include <string>

#include "Common.h" // Common

#include "wiimote_hid.h" // Local
#include "EmuDefinitions.h"
#include "Encryption.h"
#include "Console.h" // for startConsoleWin, wprintf, GetConsoleHwnd
////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
// Declarations and definitions
// ���������
extern SWiimoteInitialize g_WiimoteInitialize;
//extern void __Log(int log, const char *format, ...);
//extern void __Log(int log, int v, const char *format, ...);
///////////////////////////////


namespace WiiMoteEmu
{

void HidOutputReport(u16 _channelID, wm_report* sr);

void WmLeds(u16 _channelID, wm_leds* leds);
void WmReadData(u16 _channelID, wm_read_data* rd);
void WmWriteData(u16 _channelID, wm_write_data* wd);
void WmRequestStatus(u16 _channelID, wm_request_status* rs);
void WmRequestStatus_(u16 _channelID, int a);
void WmDataReporting(u16 _channelID, wm_data_reporting* dr);

void SendReadDataReply(u16 _channelID, void* _Base, u16 _Address, u8 _Size);
void SendReportCoreAccel(u16 _channelID);
void SendReportCoreAccelIr12(u16 _channelID);
void SendReportCore(u16 _channelID);
void SendReportCoreAccelExt16(u16 _channelID);
void SendReportCoreAccelIr10Ext(u16 _channelID);

int WriteWmReport(u8* dst, u8 channel);
void WmSendAck(u16 _channelID, u8 _reportID, u32 address);

void FillReportAcc(wm_accel& _acc);
void FillReportInfo(wm_core& _core);
void FillReportIR(wm_ir_extended& _ir0, wm_ir_extended& _ir1);
void FillReportIRBasic(wm_ir_basic& _ir0, wm_ir_basic& _ir1);
void FillReportExtension(wm_extension& _ext);
void FillReportClassicExtension(wm_classic_extension& _ext);

} // namespace

#endif	//_EMU_DECLARATIONS_
