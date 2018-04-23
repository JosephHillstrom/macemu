/*
 *  main.cpp - Startup/shutdown code
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "sysdeps.h"

#include "cpu_emulation.h"
#include "xpram.h"
#include "timer.h"
#include "sony.h"
#include "disk.h"
#include "cdrom.h"
#include "scsi.h"
#include "extfs.h"
#include "audio.h"
#include "video.h"
#include "serial.h"
#include "ether.h"
#include "clip.h"
#include "adb.h"
#include "rom_patches.h"
#include "user_strings.h"
#include "prefs.h"
#include "main.h"

#define DEBUG 0
#include "debug.h"

#if ENABLE_MON
#include "mon.h"

static uint32 mon_read_byte_b2(uintptr adr)
{
	return ReadMacInt8(adr);
}

static void mon_write_byte_b2(uintptr adr, uint32 b)
{
	WriteMacInt8(adr, b);
}
#endif

char* romInfoStr = { 0 };

void fillROMInfo()
{
	// get some information about the ROM
	if ( !romInfoStr || strlen( romInfoStr ) < 1 )
	{
		romInfoStr = (char*)malloc( 1 << 16 );
	}

	get_rom_info( romInfoStr );
}

char* getROMInfo()
{
	return romInfoStr;
}

/*
 *  Print ROM information to buf
 *
 *	note: provide big symbol buffers
 */

size_t list_rom_resources(const char* buf)
{
	char* b = (char*)buf;
	size_t len;

	const char* resourcesStr = "ROM Resources";
	len = strlen( resourcesStr );
	memcpy( b, resourcesStr, len );
	b += len;
	*b = '\n'; b++;
	*b = '\n'; b++;
	const char* tableHeadStr = "     Offset   Type  ID      Size      Name";
	len = strlen( tableHeadStr );
	memcpy( b, tableHeadStr, len );
	b += len;
	*b = '\n'; b++;
	*b = '\n'; b++;

	uint32 lp = ROMBaseMac + ReadMacInt32(ROMBaseMac + 0x1a);
	uint32 rsrc_ptr = ReadMacInt32(lp);

	for (;;) {
		lp = ROMBaseMac + rsrc_ptr;
		uint32 data = ReadMacInt32(lp + 12);

		char name[32];
		int name_len = ReadMacInt8(lp + 23), i;
		for ( i = 0; i < name_len; i++ )
			name[i] = ReadMacInt8(lp + 24 + i);
		name[i] = 0;

		char resId[32];
		sprintf( resId, "%d", ReadMacInt16(lp + 20) );
		len = strlen( resId );
		while ( len < 8 )
		{
			resId[len] = ' ';
			resId[len+1] = 0;
			len++;
		}
		char resLen[32];
		sprintf( resLen, "%d", ReadMacInt32(ROMBaseMac + data - 8) );
		len = strlen( resLen );
		while ( len < 10 )
		{
			resLen[len] = ' ';
			resLen[len+1] = 0;
			len++;
		}

		char s[256];
		sprintf( s,
				"    %08x  %c%c%c%c  %s%s%s",
				data,
				ReadMacInt8(lp + 16),
				ReadMacInt8(lp + 17),
				ReadMacInt8(lp + 18),
				ReadMacInt8(lp + 19),
				resId,
				resLen,
				name );
		len = strlen( s );
		memcpy( b, s, len );
		b += len;
		*b = '\n'; b++;

		rsrc_ptr = ReadMacInt32(lp + 8);
		if ( !rsrc_ptr )
			break;
	}

	*b = 0;
	return b - buf;
}

size_t print_universal_info(uint32 info, const char* buf)
{
	char* b = (char*)buf;
	size_t len;

	uint8 id = ReadMacInt8(info + 18);
	uint16 hwcfg = ReadMacInt16(info + 16);
	uint16 rom85 = ReadMacInt16(info + 20);

	// Find model name
	const char *name = "unknown";
	for ( int i = 0; macDesc[i].id >= 0; i++ )
	{
		if ( macDesc[i].id == id + 6 ) {
			name = macDesc[i].name;
			break;
		}
	}

	char s[256];
	sprintf( s, "    %08x  %02x  %04x  %04x  %s", info - ROMBaseMac, id, hwcfg, rom85, name );
	len = strlen( s );
	memcpy( b, s, len );
	b += len;
	*b = '\n'; b++;

	*b = 0;
	return b - buf;
}

size_t list_universal_infos(const char* buf)
{
	char* b = (char*)buf;
	size_t len;
	char s[256];

	uint32 ofs = 0x3000;
	for (int i=0; i<0x2000; i+=2, ofs+=2) {
		if (ReadMacInt32(ROMBaseMac + ofs) == 0xdc000505) {
			ofs -= 16;
			uint32 q;
			for (q=ofs; q > 0 && ReadMacInt32(ROMBaseMac + q) != ofs - q; q-=4) ;
			if (q > 0) {
				sprintf( s, "Universal Table at %08x", q );
				len = strlen( s );
				memcpy( b, s, len );
				b += len;
				*b = '\n'; b++;
				*b = '\n'; b++;
				const char* tableHeadStr = "     Offset   ID  HWCfg ROM85 Model";
				len = strlen( tableHeadStr );
				memcpy( b, tableHeadStr, len );
				b += len;
				*b = '\n'; b++;
				while ((ofs = ReadMacInt32(ROMBaseMac + q))) {
					len = print_universal_info( ROMBaseMac + ofs + q, b );
					b += len;
					q += 4;
				}
			}
			break;
		}
	}

	*b = 0;
	return b - buf;
}

size_t get_rom_info(const char* buf)
{
	char* b = (char*)buf;
	size_t len;
	char s[256];

	*b = '\n'; b++;
	const char* infoStr = "ROM Info";
	len = strlen( infoStr );
	memcpy( b, infoStr, len );
	b += len;
	*b = '\n'; b++;
	*b = '\n'; b++;

	sprintf( s, "    Checksum        %08x", ReadMacInt32(ROMBaseMac) );
	len = strlen( s );
	memcpy( b, s, len );
	b += len;
	*b = '\n'; b++;
	sprintf( s, "    Version         %04x", ROMVersion );
	len = strlen( s );
	memcpy( b, s, len );
	b += len;
	*b = '\n'; b++;
	sprintf( s, "    Sub Version     %04x", ReadMacInt16(ROMBaseMac + 18) );
	len = strlen( s );
	memcpy( b, s, len );
	b += len;
	*b = '\n'; b++;
	sprintf( s, "    Resource Map    %08x", ReadMacInt32(ROMBaseMac + 26) );
	len = strlen( s );
	memcpy( b, s, len );
	b += len;
	*b = '\n'; b++;
	sprintf( s, "    Trap Tables     %08x", ReadMacInt32(ROMBaseMac + 34) );
	len = strlen( s );
	memcpy( b, s, len );
	b += len;
	*b = '\n'; b++;
	*b = '\n'; b++;

	if ( ROMVersion == ROM_VERSION_32 ) {
		len = list_rom_resources( b );
		b += len;
		*b = '\n'; b++;
		len = list_universal_infos( b );
		b += len;
		*b = '\n'; b++;
	}

	*b = 0;
	return b - buf;
}

/*
 *  Initialize everything, returns false on error
 */

bool InitAll(const char *vmdir)
{
	// Check ROM version
	if (!CheckROM()) {
		ErrorAlert(STR_UNSUPPORTED_ROM_TYPE_ERR);
		return false;
	}

#if EMULATED_68K
	// Set CPU and FPU type (UAE emulation)
	switch (ROMVersion) {
		case ROM_VERSION_64K:
		case ROM_VERSION_PLUS:
		case ROM_VERSION_CLASSIC:
			CPUType = 0;
			FPUType = 0;
			//setTwentyFourBitAddressingByComputer( true );
			break;
		case ROM_VERSION_II:
			CPUType = PrefsFindInt32("cpu");
			if (CPUType < 2) CPUType = 2;
			if (CPUType > 4) CPUType = 4;
			FPUType = PrefsFindBool("fpu") ? 1 : 0;
			if (CPUType == 4) FPUType = 1;	// 68040 always with FPU
			//setTwentyFourBitAddressingByComputer( true );
			break;
		case ROM_VERSION_32:
			CPUType = PrefsFindInt32("cpu");
			if (CPUType < 2) CPUType = 2;
			if (CPUType > 4) CPUType = 4;
			FPUType = PrefsFindBool("fpu") ? 1 : 0;
			if (CPUType == 4) FPUType = 1;	// 68040 always with FPU
			//setTwentyFourBitAddressingByComputer( false );
			break;
	}

	CPUIs68060 = false;

#endif

	// Load XPRAM
	XPRAMInit(vmdir);

	// Load XPRAM default values if signature not found
	if (XPRAM[0x0c] != 0x4e || XPRAM[0x0d] != 0x75
	 || XPRAM[0x0e] != 0x4d || XPRAM[0x0f] != 0x63) {
		D(bug("Loading XPRAM default values\n"));
		memset(XPRAM, 0, 0x100);
		XPRAM[0x0c] = 0x4e;	// "NuMc" signature
		XPRAM[0x0d] = 0x75;
		XPRAM[0x0e] = 0x4d;
		XPRAM[0x0f] = 0x63;
		XPRAM[0x01] = 0x80;	// InternalWaitFlags = DynWait (don't wait for SCSI devices upon bootup)
		XPRAM[0x10] = 0xa8;	// Standard PRAM values
		XPRAM[0x11] = 0x00;
		XPRAM[0x12] = 0x00;
		XPRAM[0x13] = 0x22;
		XPRAM[0x14] = 0xcc;
		XPRAM[0x15] = 0x0a;
		XPRAM[0x16] = 0xcc;
		XPRAM[0x17] = 0x0a;
		XPRAM[0x1c] = 0x00;
		XPRAM[0x1d] = 0x02;
		XPRAM[0x1e] = 0x63;
		XPRAM[0x1f] = 0x00;
		XPRAM[0x08] = 0x13;
		XPRAM[0x09] = 0x88;
		XPRAM[0x0a] = 0x00;
		XPRAM[0x0b] = 0xcc;
		XPRAM[0x76] = 0x00;	// OSDefault = MacOS
		XPRAM[0x77] = 0x01;
	}

	// Set boot volume
	int16 i16 = PrefsFindInt32("bootdrive");
	XPRAM[0x78] = i16 >> 8;
	XPRAM[0x79] = i16 & 0xff;
	i16 = PrefsFindInt32("bootdriver");
	XPRAM[0x7a] = i16 >> 8;
	XPRAM[0x7b] = i16 & 0xff;

	// Init drivers
	SonyInit();
	DiskInit();
	CDROMInit();
	SCSIInit();

#if SUPPORTS_EXTFS
	// Init external file system
	ExtFSInit();
#endif

	// Init serial ports
	SerialInit();

	// Init network
	EtherInit();

	// Init Time Manager
	TimerInit();

	// Init clipboard
	ClipInit();

	// Init ADB
	ADBInit();

	// Init audio
	AudioInit();

	// Init video
	if (!VideoInit(ROMVersion == ROM_VERSION_64K || ROMVersion == ROM_VERSION_PLUS || ROMVersion == ROM_VERSION_CLASSIC))
		return false;

	// Set default video mode in XPRAM
	XPRAM[0x56] = 0x42;	// 'B'
	XPRAM[0x57] = 0x32;	// '2'
	const monitor_desc &main_monitor = *VideoMonitors[0];
	XPRAM[0x58] = uint8(main_monitor.depth_to_apple_mode(main_monitor.get_current_mode().depth));
	XPRAM[0x59] = 0;

#if EMULATED_68K
	// Init 680x0 emulation (this also activates the memory system which is needed for PatchROM())
	if (!Init680x0())
		return false;
#endif

	// Install ROM patches
	if (!PatchROM()) {
		ErrorAlert(STR_UNSUPPORTED_ROM_TYPE_ERR);
		return false;
	}

#if ENABLE_MON
	// Initialize mon
	mon_init();
	mon_read_byte = mon_read_byte_b2;
	mon_write_byte = mon_write_byte_b2;
#endif

	fillROMInfo();

	updateROMInfoWindow( getROMInfo() );
	//fprintf( stdout, getROMInfo() );

	return true;
}


/*
 *  Deinitialize everything
 */

void ExitAll(void)
{
#if ENABLE_MON
	// Deinitialize mon
	mon_exit();
#endif

	// Save XPRAM
	XPRAMExit();

	// Exit video
	VideoExit();

	// Exit audio
	AudioExit();

	// Exit ADB
	ADBExit();

	// Exit clipboard
	ClipExit();

	// Exit Time Manager
	TimerExit();

	// Exit serial ports
	SerialExit();

	// Exit network
	EtherExit();

#if SUPPORTS_EXTFS
	// Exit external file system
	ExtFSExit();
#endif

	// Exit drivers
	SCSIExit();
	CDROMExit();
	DiskExit();
	SonyExit();
}


/*
 *  Display error/warning alert given the message string ID
 */

void ErrorAlert(int string_id)
{
	ErrorAlert(GetString(string_id));
}

void WarningAlert(int string_id)
{
	WarningAlert(GetString(string_id));
}
