/*
 *	Emulator.h - Class whose actions are attached GUI widgets in a window,
 *				 used to control a single Basilisk II emulated Macintosh.
 *
 *	$Id$
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

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#import "EmulatorView.h"
#import "NNThread.h"

extern void updateROMInfoWindow( char* info );
extern void updateXPRAMWindow( char* xpram_text );

@interface CocoaTextViewWithText : NSTextView
{
	NSString* myNSString;
}

- (NSString*) myNSString;

- (void) initializeWithSymbols:(char*)symbols;
- (void) updateWithSymbols:(char*)symbols;

- (id) init;
- (void) dealloc;

- (void) drawRect:(NSRect)inRect;

- (void) pasteFont:(id)sender;

- (void) setNeedsDisplay:(BOOL)needsDisplay;
@end

@interface Emulator : NSObject
{
	NNThread	*emul;			// Run emulThread
	NNTimer		*RTC,			// Invoke RTCinterrupt
				*redraw,		// Invoke redrawScreen
				*tick,			// Invoke tickInterrupt
				*xPRAM,			// Invoke xPRAMbackup
				*addressing;

	BOOL		uaeCreated,		// Has thread created the emulator environment?
				running;		// Is the emulator currently grinding away?
	float		redrawDelay;	// Seconds until next screen update

	BOOL		twentyFourBitAddressing;
	BOOL		addressingChosenByUser;

	// UI elements that this class changes the state of

	IBOutlet NSProgressIndicator	*barberPole;
	IBOutlet NSButton				*runOrPause;
	IBOutlet NSButton				*addressingMode;
	IBOutlet EmulatorView			*screen;
	IBOutlet NSSlider				*speed;
	IBOutlet NSWindow				*win;
	IBOutlet NSWindow				*romInfoWindow;
	IBOutlet NSScrollView			*romInfoView;
	IBOutlet CocoaTextViewWithText	*romInfoTextView;
	IBOutlet NSMenuItem				*showHideROMInfoWindow;
	IBOutlet NSWindow				*XPRAMWindow;
	IBOutlet NSScrollView			*XPRAMScrollView;
	IBOutlet CocoaTextViewWithText	*XPRAMTextView;
	IBOutlet NSMenuItem				*showHideXPRAMWindow;
}

extern Emulator*	currentEmulator;

+ (Emulator*) currentEmulator;

// The following allow the Controller and PrefsEditor classes to access our internal data

- (BOOL)			isRunning;
- (BOOL)			uaeCreated;
- (EmulatorView *)	screen;
- (NSSlider *)		speed;
- (NSWindow *)		window;

- (void) runUpdate;		// Update some UI elements

- (IBAction) Benchmark:		(id)sender;
- (IBAction) Interrupt:		(id)sender;
- (IBAction) PowerKey:		(id)sender;
- (IBAction) Restart:		(id)sender;
- (IBAction) Resume:		(id)sender;
- (IBAction) Snapshot:		(id)sender;
- (IBAction) SpeedChange:	(NSSlider *)sender;
- (IBAction) Suspend:		(id)sender;
- (IBAction) Terminate:		(id)sender;
- (IBAction) ToggleState:	(NSButton *)sender;
- (IBAction) ZapPRAM:		(id)sender;

- (IBAction) showOrHideROMInfo:	(id)sender;
- (void) showROMInfo:	(id)sender;
- (void) hideROMInfo:	(id)sender;
- (void) updateMenuItemForROMInfo;

- (IBAction) showOrHideXPRAM:	(id)sender;
- (void) hideXPRAMDump:	(id)sender;
- (void) showXPRAMDump:	(id)sender;
- (void) updateMenuItemForXPRAM;

- (void)makeKeyAndOrderFront:(id)sender;

- (void) createThreads;
- (void) exitThreads;

- (void) emulThread;			// Thread for processor emulator
- (void) RTCinterrupt;			// Emulator real time clock update
- (void) redrawScreen;			// Draw emulator screen in window
- (void) tickInterrupt;
- (void) xPRAMbackup;			// PRAM watchdog
- (void) addressingThread;		// addressing mode watchdog

- (void) setTwentyFourBitAddressing:(BOOL)addressing24;
- (void) setTwentyFourBitAddressingByComputer:(BOOL)addressing24;
- (void) setThirtyTwoBitAddressing:(BOOL)addressing32;
- (IBAction) toggleTwentyFourBitAddressing:(id)sender;
- (BOOL) twentyFourBitAddressing;
- (BOOL) addressingChosenByUser;
- (void) setAddressingChosenByUser:(BOOL)userSet;
- (void) updateAddressingButton;
- (IBAction) doUpdateAddressingButton:(id)sender;

- (BOOL) is24bitAddrInXPRAM;
- (void) set24bitAddrInXPRAM:(BOOL)mode24;

- (NSTextView*) romInfoTextView;
- (NSScrollView*) romInfoView;

- (NSTextView*) XPRAMTextView;
- (NSScrollView*) XPRAMScrollView;

- (void) updateROMInfoWindowWith:(char*) info;
- (void) updateXPRAMWindowWith: (char*) xpram_text;

@end
