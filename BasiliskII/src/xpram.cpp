/*
 *  xpram.cpp - XPRAM handling
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

/*
 *  SEE ALSO
 *    Inside Macintosh: Operating System Utilities, chapter 7 "Parameter RAM Utilities"
 */

#include <string.h>

#include "CrossPlatform/sysdeps.h"
#include "xpram.h"


// Extended parameter RAM
uint8 XPRAM[XPRAM_SIZE];

char* xpramStr = { 0 };

char* dumpXPRAM()
{
	if ( !xpramStr || strlen( xpramStr ) < 1 )
	{
		xpramStr = (char*)malloc( 1 << 16 );
	}

	XPRAMtoString( xpramStr );
	return xpramStr;
}

/*
 *  Initialize XPRAM
 */

void XPRAMInit(const char *vmdir)
{
	// Clear XPRAM
	memset(XPRAM, 0, XPRAM_SIZE);

	// Load XPRAM from settings file
	LoadXPRAM(vmdir);
}


/*
 *  Deinitialize XPRAM
 */

void XPRAMExit(void)
{
	// Save XPRAM to settings file
	SaveXPRAM();
}

size_t XPRAMtoString( const char* buf )
{
	char* b = (char*)buf;
	size_t len;

	*b = '\n'; b++;
	const char* str = "XPRAM";
	len = strlen( str );
	memcpy( b, str, len );
	b += len;
	*b = '\n'; b++;
	*b = '\n'; b++;

	char s[256];
	for ( size_t offset = 0; offset < XPRAM_SIZE; offset += 16 ) {
		sprintf( s, "    %08x", offset );
		len = strlen( s );
		memcpy( b, s, len );
		b += len;
		*b = ' '; b++;
		*b = ' '; b++;

		//char ascii[17];
		for ( size_t bytePos = 0; bytePos < 16; bytePos++ )
		{
			uint8 symbol = XPRAM[offset + bytePos];
			//if ( symbol >= 32 && symbol < 128 )
			//	ascii[bytePos] = (char)symbol;
			//else
			//	ascii[bytePos] = ' ';
			sprintf( s, "%02x", symbol );
			len = strlen( s );
			memcpy( b, s, len );
			b += len;
			*b = ' '; b++;
		}
		//*b = ' '; b++;
		//memcpy( b, ascii, 16 );
		//b += 16;

		*b = '\n'; b++;
	}

	*b = 0;
	return b - buf;
}

bool getXPRAMat(size_t off, uint8* out)
{
	if ( !out )
		return false;
	if ( off >= XPRAM_SIZE )
		return false;

	*out = XPRAM[ off ];
	return true;
}

bool setXPRAMat(size_t off, uint8 byte)
{
	if ( off >= XPRAM_SIZE )
		return false;

	XPRAM[ off ] = byte;
	return true;
}
