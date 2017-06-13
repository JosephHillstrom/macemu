/*
 *	Emulator.mm - Class whose actions are attached to GUI widgets in a window,
 *				  used to control a single Basilisk II emulated Macintosh.
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

#import "Emulator.h"
#import "EmulatorView.h"

#import "sysdeps.h"			// Types used in Basilisk C++ code

#import "main_macosx.h"		// Prototypes for QuitEmuNoExit() and InitEmulator()
#import "misc_macosx.h"		// Some other prototypes
#import "video_macosx.h"	// Some window/view globals

#import "adb.h"
#import "main.h"
#import "prefs.h"
#import "timer.h"
#import "xpram.h"

#undef check				// memory.h defines a check macro,
							// which may clash with an OS X one on 10.1 or 10.2
#import "cpu_emulation.h"
#import "rom_patches.h"

#define DEBUG 0
#import "debug.h"

Emulator* theCurrentEmulator;

void updateXPRAMWindow( char* symbols )
{
	[ [ Emulator currentEmulator ] updateXPRAMWindowWith:symbols ];
}

void updateROMInfoWindow( char* symbols )
{
	[ [ Emulator currentEmulator ] updateROMInfoWindowWith:symbols ];
}

@implementation CocoaTextViewWithText

- (NSString*) myNSString
{
	return myNSString;
}

- (void) initializeWithSymbols:(char*)symbols
{
	//myText = nil;
	myNSString = nil;

	//NSColor* theColor = [[ NSColor alloc ] colorWithCalibratedWhite:0 alpha:1 ];
	/*NSColor* theColor = [[ NSColor alloc ] colorWithCalibratedRed:0.0f/255.0f
															green:40.0f/255.0f
															 blue:40.0f/255.0f
															alpha:0.8f ];
	[ self setTextColor:theColor ];*/
	/*
	NSShadow* theShadow = [[ NSShadow alloc ] init ];
	[ theShadow setShadowColor: shadowColor ];
	[ theShadow setShadowOffset: NSMakeSize(2, -1) ];
	[ theShadow setShadowBlurRadius: 2.2 ];
	*/

	[ self updateWithSymbols:symbols ];
}

- (void) updateWithSymbols:(char*)symbols
{
	if ( symbols )
	{
		myNSString = [ NSString stringWithUTF8String:symbols ];
		[ self setString: myNSString ];
		//[[ self textStorage ] setFont: [ NSFont fontWithName:@"Monaco" size:9 ] ];
		[ self setSelectedRange: NSMakeRange(0, 0) ];
	}
	else
	{
		[ self setString:@"" ];
	}

	[ self setNeedsDisplay:YES ];
}

- (id)init
{
    if( self = [super init] )
    {
		myNSString = nil;

		char emptyString[] = { 0 };
		[ self initializeWithSymbols:emptyString ];
    }

    return self;
}

- (void) dealloc
{
	if ( myNSString )
		[ myNSString release ];
	myNSString = nil;

	[ super dealloc ];
}

- (void) drawRect:(NSRect)inRect
{
	[ super drawRect:inRect ];
}

- (void) pasteFont:(id)sender
{
	NSRange range = [ self selectedRange ];
	BOOL wasRichText = [ self isRichText ];
	if ( !wasRichText )
		[ self setRichText: YES ];
	[ self selectAll: nil ];
	[ super pasteFont: sender ];
	if ( !wasRichText )
		[ self setRichText: NO ];
	[ self setSelectedRange: range ];
}

- (void) setNeedsDisplay:(BOOL)needsDisplay
{
	[ super setNeedsDisplay:needsDisplay ];
}

@end

@implementation Emulator

+ (Emulator*) currentEmulator
{
	return theCurrentEmulator;
}

// NSWindow method, which is invoked via delegation

- (BOOL) windowShouldClose: (id)sender
{
	if ( ![sender isKindOfClass:[ NSWindow class ]] )
        return NO;

	NSWindow* senderWindow = (NSWindow*) sender;

	if ( senderWindow == XPRAMWindow )
	{
		[ self hideXPRAMDump:sender ];
		return NO;
	}

	if ( senderWindow == romInfoWindow )
	{
		[ self hideROMInfo:sender ];
		return NO;
	}

	if ( senderWindow == [ self window ] )
	{
		if ( uaeCreated )
			return NO;

		return YES;
	}

	return NO;
}

// Default methods

- (Emulator *) init
{
	int frameSkip;

	self = [super init];

	running = NO;			// Save churn when application loads
//	running = YES;
	uaeCreated = NO;

	frameSkip = PrefsFindInt32("frameskip");
	if ( frameSkip )
		redrawDelay = frameSkip / 60.0;
	else
		redrawDelay = 0.0;

	// We do this so that we can work out if we are in full screen mode:
	parse_screen_prefs(PrefsFindString("screen"));

	[self createThreads];

	return self;
}

- (void) awakeFromNib
{
	theCurrentEmulator = self;

	the_win = win;					// global for access by Basilisk C++ code

	// enable windowShouldClose
	[win setDelegate: self];

	// Try to speed up everything
	//[win setHasShadow: NO];		// This causes view & window to now be drawn correctly
	[win useOptimizedDrawing: YES];

	[win makeKeyAndOrderFront:self];

	if ( redrawDelay )
		[speed setFloatValue: 1.0 / redrawDelay];
	else
		[speed setFloatValue: 60.0];

	if ( runOrPause == nil )
		NSLog(@"%s - runOrPause button pointer is nil", __PRETTY_FUNCTION__);

	addressingChosenByUser = NO;

	[ self updateAddressingButton ];

	NSScreen* mainScreen = [ NSScreen mainScreen ];
	NSRect screenRect = [ mainScreen visibleFrame ];

	{
		CGFloat windowWidth = 340;
		CGFloat windowHeight = screenRect.size.height - 20;
		CGFloat horizontalPositionOfWindow = 8;
		CGFloat verticalPositionOfWindow = screenRect.size.height - windowHeight - 10;
		NSRect theRect = NSMakeRect(horizontalPositionOfWindow, verticalPositionOfWindow, windowWidth, windowHeight);
		NSUInteger windStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
		NSRect windowRect = [ NSWindow contentRectForFrameRect:theRect styleMask:windStyleMask ];

		// create the ROM Info window
		romInfoWindow = [ [NSWindow alloc] initWithContentRect:windowRect styleMask:windStyleMask backing: NSBackingStoreBuffered defer:NO ];
		[ romInfoWindow setTitle:@"ROM Info" ];
		[ romInfoWindow setBackgroundColor:[ NSColor whiteColor ] ];
		[ romInfoWindow setDelegate: self ]; // enable windowShouldClose

		// create the ROM Info text view
		romInfoTextView = [ [CocoaTextViewWithText alloc] initWithFrame:theRect ];
		[ romInfoTextView setBounds:theRect ];

		// create the scroll view so that it fills the entire window
		romInfoView = [[NSScrollView alloc] initWithFrame:theRect ];
		[ romInfoView setHasVerticalScroller:YES ];
		[ romInfoView setHasHorizontalScroller:NO ];
		[ romInfoView setAutohidesScrollers:NO ];
		[ romInfoView setBorderType:NSNoBorder ];
		[ romInfoView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable) ];

		// set romInfoTextView as the documentView of the scroll view
		[ romInfoView setDocumentView:romInfoTextView ];
		// set the romInfoView as the window's contentView
		[ romInfoWindow setContentView:romInfoView ];

		[ romInfoTextView setEditable:NO ];
		[ romInfoTextView setString:@"" ];

		[ romInfoWindow setIsVisible:NO ];
		//[ romInfoTextView setNeedsDisplay:YES ];
		//[ romInfoView setNeedsDisplay:YES ];

		[ showHideROMInfoWindow setEnabled:YES ];
		[ self updateMenuItemForROMInfo ];
	}

	{
		CGFloat windowWidth = 420;
		CGFloat windowHeight = 310;
		CGFloat horizontalPositionOfWindow = screenRect.size.width - windowWidth;
		CGFloat verticalPositionOfWindow = screenRect.size.height - windowHeight;
		NSRect theRect = NSMakeRect(horizontalPositionOfWindow, verticalPositionOfWindow, windowWidth, windowHeight);
		NSUInteger windStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
		NSRect windowRect = [ NSWindow contentRectForFrameRect:theRect styleMask:windStyleMask ];

		// create the XPRAM Dump window
		XPRAMWindow = [ [NSWindow alloc] initWithContentRect:windowRect styleMask:windStyleMask backing: NSBackingStoreBuffered defer:NO ];
		[ XPRAMWindow setTitle:@"XPRAM" ];
		[ XPRAMWindow setBackgroundColor:[ NSColor whiteColor ] ];
		[ XPRAMWindow setDelegate: self ]; // enable windowShouldClose

		// create the XPRAM Dump text view
		XPRAMTextView = [ [CocoaTextViewWithText alloc] initWithFrame:theRect ];
		[ XPRAMTextView setBounds:theRect ];

		// create the scroll view so that it fills the entire window
		XPRAMScrollView = [[NSScrollView alloc] initWithFrame:theRect ];
		[ XPRAMScrollView setHasVerticalScroller:YES ];
		[ XPRAMScrollView setHasHorizontalScroller:NO ];
		[ XPRAMScrollView setAutohidesScrollers:NO ];
		[ XPRAMScrollView setBorderType:NSNoBorder ];
		[ XPRAMScrollView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable) ];

		// set XPRAMTextView as the documentView of the scroll view
		[ XPRAMScrollView setDocumentView:XPRAMTextView ];
		// set the XPRAMScrollView as the window's contentView
		[ XPRAMWindow setContentView:XPRAMScrollView ];

		[ XPRAMTextView setEditable:NO ];
		[ XPRAMTextView setString:@"" ];

		[ self showXPRAMDump: /*(id)sender*/ nil ];
		//[ XPRAMTextView setNeedsDisplay:YES ];
		//[ XPRAMScrollView setNeedsDisplay:YES ];

		[ showHideXPRAMWindow setEnabled:YES ];
		[ self updateMenuItemForXPRAM ];
	}

	[ self runUpdate ];
}


// Helpers which other classes use to access our private stuff

- (BOOL) isRunning	{	return running;		}

- (BOOL) uaeCreated
{
	return uaeCreated;
}

- (EmulatorView *)	screen		{	return screen;		}
- (NSSlider *)		speed		{	return speed;		}
- (NSWindow *)		window		{	return win;			}


// Update some UI elements

- (void) runUpdate
{
	if ( running )
		[runOrPause setState: NSOnState];	// Running. Change button label to pause
	else
		[runOrPause setState: NSOffState];	// Paused.  Change button label to go

	[win setDocumentEdited: uaeCreated];	// Set the little dimple in the close button
}


// Methods invoked by buttons & menu items

- (IBAction) Benchmark:	(id)sender;
{
	BOOL	wasRunning = running;

	if ( running )
		[self Suspend: self];
	[screen benchmark];
	if ( wasRunning )
		[self Resume: self];
}

#ifdef NIGEL
- (IBAction) EjectCD: (id)sender;
{
	NSString	*path;
	const char	*cdrom = PrefsFindString("cdrom");

	if ( cdrom )
	{
	#include <sys/param.h>
	#define KERNEL
	#include <sys/mount.h>

		struct statfs buf;
		if ( fsstat(path, &buf) < 0 )
			return;

		path = [NSString stringWithCString: cdrom];

		[[NSWorkspace sharedWorkspace] unmountAndEjectDeviceAtPath: path];
//		[path release];
	}
}
#endif

- (IBAction) Interrupt:	(id)sender;
{
	//WarningSheet (@"Interrupt action not yet supported", win);

	SetInterruptFlag(INTFLAG_NMI);
	ADBKeyDown(0x7f);	// it is power key which is also named ADB_RESET or ADB_POWER
	ADBKeyDown(0x37);	// command+power key is for non-maskable interrupt
	TriggerInterrupt();
	ADBKeyUp(0x7f);		// release power key
	ADBKeyUp(0x37);		// release command
	/*
	0x36	control
	0x37	command
	0x38	shift
	0x3a	option (alt)
	*/
}

- (IBAction) PowerKey:	(id)sender;
{
	if ( uaeCreated )		// If Mac has started
	{
		ADBKeyDown(0x7f);	// Send power key, which is also
		ADBKeyUp(0x7f);		// called ADB_RESET or ADB_POWER
	}
	else
	{
		running = YES;						// Start emulator
		[self runUpdate];
		[self Resume: nil];
	}
}

- (IBAction) Restart: (id)sender
{
	if ( ! running )
	{
		running = YES;						// Start emulator
		[self runUpdate];
		[self Resume: nil];
	}

	if ( running )
#ifdef UAE_CPU_HAS_RESET
		reset680x0();
#else
	{
		uaeCreated = NO;
		[redraw suspend];
		NSLog (@"%s - uae_cpu reset not yet supported, will try to fake it",
				__PRETTY_FUNCTION__);

		[screen clear];
		[screen display];

		[emul terminate]; QuitEmuNoExit();


		// OK. We have killed & cleaned up. Now, start afresh:
	#include <sys.h>
		int	argc = 0;
		char **argv;

		PrefsInit(NULL, argc, argv);
		SysInit();

		emul = [NNThread new];
		[emul perform:@selector(emulThread) of:self];
		[emul start];

		if ( display_type != DISPLAY_SCREEN )
			[redraw resume];
	}
#endif
}

- (IBAction) Resume: (id)sender
{
	[RTC	resume];
	[emul	resume];
	if ( display_type != DISPLAY_SCREEN )
		[redraw	resume];
	[tick	resume];
	[xPRAM	resume];
	[addressing resume];
}

- (IBAction) Snapshot: (id) sender
{
	if ( screen == nil || uaeCreated == NO  )
		WarningSheet(@"The emulator has not yet started.",
					 @"There is no screen output to snapshot",
					 nil, win);
	else
	{
		NSData	*TIFFdata;

		[self Suspend: self];

		TIFFdata = [ screen TIFFrep ];
		NSString* fileName = @"screensnap.tiff";
		if ( TIFFdata == nil )
		{
			NSLog(@"%s - Unable to convert Basilisk screen to a TIFF representation",
					__PRETTY_FUNCTION__);
			TIFFdata = [ [ NSData alloc ] initWithBytes:[screen bitmap] length:([screen width] * [screen height] / 8) ];
			fileName = @"screensnap.raw";
		}

		NSSavePanel *sp = [[ NSSavePanel savePanel ] retain];

		//[sp setRequiredFileType:@"tiff"];

		if ( [ sp runModalForDirectory:nil file:fileName ] == NSOKButton )
			if ( ! [ TIFFdata writeToFile:[ sp filename ] atomically:YES ] )
				NSLog(@"%s - Could not write TIFF data to file @%",
						__PRETTY_FUNCTION__, [sp filename]);
		[sp release];

		if ( running )
			[self Resume: self];
	}
}

- (IBAction) SpeedChange: (NSSlider *)sender
{
	float frequency = [sender floatValue];

	[redraw suspend];

	if ( frequency == 0.0 )
		redrawDelay = 0.0;
	else
	{
		frequencyToTickDelay(frequency);

		redrawDelay = 1.0 / frequency;

		[redraw changeIntervalTo: (int)(redrawDelay * 1e6)
						   units: NNmicroSeconds];
		if ( running && display_type != DISPLAY_SCREEN )
			[redraw resume];
	}
}

- (IBAction) Suspend: (id)sender
{
	[RTC	suspend];
	[emul	suspend];
	[redraw	suspend];
	[tick	suspend];
	[xPRAM	suspend];
	[addressing suspend];
}

- (IBAction) ToggleState: (NSButton *)sender
{
	running = [sender state];		// State of the toggled NSButton
	if ( running )
	{
		[self Resume: nil];
	}
	else
	{
		[self Suspend: nil];
	}
}

- (IBAction) Terminate: (id)sender;
{
	[self exitThreads];
	[win performClose: self];
}

- (void)makeKeyAndOrderFront:(id)sender
{
}

#include <xpram.h>

uint8 lastXPRAM[XPRAM_SIZE];		// Copy of PRAM

- (IBAction) ZapPRAM: (id)sender;
{
	memset(XPRAM,     0, XPRAM_SIZE);
	memset(lastXPRAM, 0, XPRAM_SIZE);
	ZapPRAM();
}

- (void) updateMenuItemForROMInfo
{
	if ( [ romInfoWindow isVisible ] )
		[ showHideROMInfoWindow setTitle:@"Hide ROM Info" ];
	else
		[ showHideROMInfoWindow setTitle:@"Show ROM Info" ];
}

- (IBAction) showOrHideROMInfo: (id)sender
{
	if ( [ romInfoWindow isVisible ] )
		[ self hideROMInfo:sender ];
	else
		[ self showROMInfo:sender ];
}

- (void) hideROMInfo: (id)sender
{
	[ romInfoWindow setIsVisible:NO ];
	[ showHideROMInfoWindow setTitle:@"Show ROM Info" ];
}

- (void) showROMInfo: (id)sender
{
	[ self updateROMInfoWindowWith: getROMInfo() ];
	//[ romInfoTextView setNeedsDisplay:YES ];
	[ romInfoView setNeedsDisplay:YES ];
	[ romInfoWindow setIsVisible:YES ];
	[ showHideROMInfoWindow setTitle:@"Hide ROM Info" ];
}

- (void) updateMenuItemForXPRAM
{
	if ( [ XPRAMWindow isVisible ] )
		[ showHideXPRAMWindow setTitle:@"Hide XPRAM" ];
	else
		[ showHideXPRAMWindow setTitle:@"Show XPRAM" ];
}

- (IBAction) showOrHideXPRAM: (id)sender
{
	if ( [ XPRAMWindow isVisible ] )
		[ self hideXPRAMDump:sender ];
	else
		[ self showXPRAMDump:sender ];
}

- (void) hideXPRAMDump: (id)sender
{
	[ XPRAMWindow setIsVisible:NO ];
	[ showHideXPRAMWindow setTitle:@"Show XPRAM" ];
}

- (void) showXPRAMDump: (id)sender
{
	[ self updateXPRAMWindowWith: dumpXPRAM() ];
	//[ XPRAMTextView setNeedsDisplay:YES ];
	[ XPRAMScrollView setNeedsDisplay:YES ];
	[ XPRAMWindow setIsVisible:YES ];
	[ showHideXPRAMWindow setTitle:@"Hide XPRAM" ];
}

//
// Threads, Timers and stuff to manage them:
//

- (void) createThreads
{
#ifdef USE_PTHREADS
	// Make UI threadsafe:
	[NSThread detachNewThreadSelector:(SEL)"" toTarget:nil withObject:nil];
	//emul   = [[NNThread	alloc] initWithAutoReleasePool];
#endif
	emul   = [NNThread	new];
	RTC    = [NNTimer	new];
	redraw = [NNTimer	new];	//[[NNTimer	alloc] initWithAutoRelPool];
	tick   = [NNTimer	new];
	xPRAM  = [NNTimer	new];
	addressing = [NNTimer new];

	[emul  perform:@selector(emulThread)	of:self];
	[RTC    repeat:@selector(RTCinterrupt)	of:self
			 every:1
			 units:NNseconds];
	[redraw	repeat:@selector(redrawScreen)	of:self
			 every:(int)(1000*redrawDelay)
			 units:NNmilliSeconds];
	[tick   repeat:@selector(tickInterrupt)	of:self
			 every:16625
			 units:NNmicroSeconds];
	[xPRAM  repeat:@selector(xPRAMbackup)	of:self
			 every:100
			 units:NNseconds];
	[addressing  repeat:@selector(addressingThread)	of:self
			 every:100
			 units:NNmilliSeconds];

	if ( running )		// Start emulator, then threads in most economical order
	{
		[emul	start];
		[xPRAM	start];
		[RTC	start];
		if ( display_type != DISPLAY_SCREEN )
			[redraw	start];
		[tick	start];
		[addressing  start];
	}
}

- (void) exitThreads
{
	running = NO;
	[emul	terminate];  [emul	 release]; emul   = nil;
	[tick	invalidate]; [tick	 release]; tick   = nil;
	[redraw	invalidate]; [redraw release]; redraw = nil;
	[RTC	invalidate]; [RTC	 release]; RTC    = nil;
	[xPRAM	invalidate]; [xPRAM	 release]; xPRAM  = nil;
	[addressing invalidate]; [addressing release]; addressing  = nil;
}

- (void) emulThread
{
	NSAutoreleasePool	*pool = [[NSAutoreleasePool alloc] init];

	if ( ! InitEmulator() )
	{
		[redraw suspend];		// Stop the barberpole

		ErrorSheet(@"Cannot start Emulator", @"", @"Quit", win);
	}
	else
	{
		memcpy(lastXPRAM, XPRAM, XPRAM_SIZE);

		uaeCreated = YES;		// Enable timers to access emulated Mac's memory
		[ self updateAddressingButton ];

		while ( screen == nil )	// If we are still loading from Nib?
			[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow: 1.0]];

		[self   runUpdate];		// Set the window close gadget to dimpled

		Start680x0();			// Start 68k and jump to ROM boot routine

		puts ("Emulator exited normally");
	}

	[pool release];
	QuitEmulator();
}

- (void) RTCinterrupt
{
	if ( ! uaeCreated )
		return;

	WriteMacInt32 (0x20c, TimerDateTime() );	// Update MacOS time

	//M68kRegisters r;
	//r.d[0] = /* trap number */ /* 0xa051 */ 0x051;		// ReadXPRam()
	//Execute68kTrap(0xa346, &r);				// GetOSTrapAddress()
	//uint32 procptrofReadXPRam = r.a[0];
	//fprintf( stdout, "procptr of ReadXPRam from GetOSTrapAddress: %08lx\n", procptrofReadXPRam );
	//r.d[0] = /* trap number */ /* 0xa052 */ 0x052;		// WriteXPRam()
	//Execute68kTrap(0xa346, &r);				// GetOSTrapAddress()
	//uint32 procptrofWriteXPRam = r.a[0];
	//fprintf( stdout, "procptr of WriteXPRam from GetOSTrapAddress: %08lx\n", procptrofWriteXPRam );

	/*
	const uint32 lowmem_OSDispTable	= 0x0400;	// 256 longs, up to $800
	const uint16 trapnumber_ReadXPRam = 0x051;	// 0xa051 ReadXPRam()
	const uint16 traptable_ReadXPRam = lowmem_OSDispTable + (trapnumber_ReadXPRam << 2);
	uint32 procptrofReadXPRam = ReadMacInt32( traptable_ReadXPRam );

	uint32 bytes1ofReadXPRam = ReadMacInt32( procptrofReadXPRam );
	uint32 bytes2ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 4 );
	uint32 bytes3ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 8 );
	uint32 bytes4ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 12 );
	uint32 bytes5ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 16 );
	uint32 bytes6ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 20 );
	uint32 bytes7ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 24 );
	uint32 bytes8ofReadXPRam = ReadMacInt32( procptrofReadXPRam + 28 );
	fprintf( stdout, "procptr @%04x of ReadXPRam: %08lx with data %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
			traptable_ReadXPRam, procptrofReadXPRam,
			bytes1ofReadXPRam, bytes2ofReadXPRam, bytes3ofReadXPRam, bytes4ofReadXPRam,
			bytes5ofReadXPRam, bytes6ofReadXPRam, bytes7ofReadXPRam, bytes8ofReadXPRam );
	const uint16 trapnumber_WriteXPRam = 0x052;	// 0xa052 WriteXPRam()
	const uint16 traptable_WriteXPRam = lowmem_OSDispTable + (trapnumber_WriteXPRam << 2);
	uint32 procptrofWriteXPRam = ReadMacInt32( traptable_WriteXPRam );

	char bytes[512];
	for ( size_t i = 0; i < 512; i++ )
		bytes[i] = ReadMacInt8( procptrofReadXPRam + i );
	char* buf = bytes;
	char dis_op[1000], dis_inst[1000], dis_arg[1000];
	int  off, j;
	for (off = bytes, j = 0; j < 1000; )
    {
        size_t k = Dis68000One(buf, &procptrofWriteXPRam[j], dis_op, dis_inst, dis_arg);

        size_t lenofop = strlen( dis_op );
        int firstoffset = getFirstOffsetOf( dis_op, '\n' );
        if ( firstoffset >= 0 ) {
			//changeSymbolTo( dis_op, '\n', '\t' );
			char firstPart[1+lenofop+28];
			char secondPart[1+lenofop];

			strncpy( firstPart, dis_op, firstoffset );
			firstPart[firstoffset] = 0;

			size_t lenof1stpart = strlen( firstPart );
			int diff = 28 - lenof1stpart;
			for ( int i = 0; i < diff; i++ ) {
				firstPart[lenof1stpart + i] = ' ';
			}
			firstPart[lenof1stpart + diff] = 0;

			strncpy( secondPart, dis_op + firstoffset + 1, lenofop - firstoffset - 1 );
			secondPart[lenofop - firstoffset - 1] = 0;

			printf( "%08X: %s %s\n%s\n", procptrofReadXPRam + buf, firstPart, dis_inst, secondPart );
        }
        else
        {
			printf( "%08X: %s %s\n", procptrofReadXPRam + buf, dis_op, dis_inst );
        }
        buf += k;
        j += k;
    }
	*/

	/*
	uint32 bytes1ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam );
	uint32 bytes2ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 4 );
	uint32 bytes3ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 8 );
	uint32 bytes4ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 12 );
	uint32 bytes5ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 16 );
	uint32 bytes6ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 20 );
	uint32 bytes7ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 24 );
	uint32 bytes8ofWriteXPRam = ReadMacInt32( procptrofWriteXPRam + 28 );
	fprintf( stdout, "procptr @%04x of WriteXPRam: %08lx with data %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
				traptable_WriteXPRam, procptrofWriteXPRam,
				bytes1ofWriteXPRam, bytes2ofWriteXPRam, bytes3ofWriteXPRam, bytes4ofWriteXPRam,
				bytes5ofWriteXPRam, bytes6ofWriteXPRam, bytes7ofWriteXPRam, bytes8ofWriteXPRam );
	*/

	SetInterruptFlag(INTFLAG_1HZ);
	TriggerInterrupt();
}

- (void) redrawScreen
{
	if ( display_type == DISPLAY_SCREEN )
	{
		NSLog(@"We are in fullscreen mode - why was redrawScreen() called?");
		return;
	}
	[barberPole animate:self];			// wobble the pole
	[screen setNeedsDisplay: YES];		// redisplay next time through runLoop
	// Or, use a direct method. e.g.
	//	[screen display] or [screen cgDrawInto: ...];
	//[screen display];
}

#include <main.h>				// For #define INTFLAG_60HZ
#include <rom_patches.h>		// For ROMVersion
#include "macos_util_macosx.h"	// For HasMacStarted()

- (void) tickInterrupt
{
	if ( ROMVersion != ROM_VERSION_CLASSIC || HasMacStarted() )
	{
		SetInterruptFlag (INTFLAG_60HZ);
		TriggerInterrupt ();
	}
}

- (void) setTwentyFourBitAddressing:(BOOL)addressing24
{
	twentyFourBitAddressing = addressing24 ? YES : NO;
	[ self set24bitAddrInXPRAM:twentyFourBitAddressing ];
	[ self updateAddressingButton ];
}

- (void) setTwentyFourBitAddressingByComputer:(BOOL)addressing24
{
	//if ( ! [ self addressingChosenByUser ] )
		[ self setTwentyFourBitAddressing:addressing24 ];
}

- (void) setThirtyTwoBitAddressing:(BOOL)addressing32
{
	twentyFourBitAddressing = addressing32 ? NO : YES;
	[ self set24bitAddrInXPRAM:twentyFourBitAddressing ];
	[ self updateAddressingButton ];
}

- (IBAction) toggleTwentyFourBitAddressing:(id)sender
{
	if ( uaeCreated == NO )
	{
		twentyFourBitAddressing = twentyFourBitAddressing ? NO : YES;
		[ self set24bitAddrInXPRAM:twentyFourBitAddressing ];
		[ self doUpdateAddressingButton:sender ];
	}
	else
	{
		[ addressingMode setEnabled:false ];
	}
	addressingChosenByUser = YES;
}

- (BOOL) twentyFourBitAddressing
{
	return twentyFourBitAddressing;
}

- (BOOL) addressingChosenByUser
{
	return addressingChosenByUser;
}

- (void) setAddressingChosenByUser:(BOOL)userSet
{
	addressingChosenByUser = userSet;
}

- (void) updateAddressingButton
{
	NSAutoreleasePool *pool;
    pool = [[NSAutoreleasePool alloc] init];

	NSString* label;
#if REAL_ADDRESSING
	label = @"real 32-bit addressing";
	[ addressingMode setBordered:false ];
	[ addressingMode setEnabled:false ];
#elif DIRECT_ADDRESSING
	label = @"direct 32-bit addressing";
	[ addressingMode setBordered:false ];
	[ addressingMode setEnabled:false ];
#else
	[ addressingMode setBordered:true ];
	if ( twentyFourBitAddressing ) {
		[ self set24bitAddrInXPRAM:YES ];
		label = @"24-bit addressing";
	} else {
		[ self set24bitAddrInXPRAM:NO ];
		label = @"32-bit addressing";
	}
#endif
	[ addressingMode setTitle:label ];
	if ( uaeCreated == YES )
		[ addressingMode setEnabled:false ];

	[ pool drain ];
}

- (IBAction) doUpdateAddressingButton:(id)sender
{
	[ self updateAddressingButton ];
}

- (BOOL) is24bitAddrInXPRAM
{
	uint8 byte;
	BOOL okay = getXPRAMat(0x8a, &byte);

	if ( byte & 0x05 == 0 )
		return YES;
	else
		return NO;
}

- (void) set24bitAddrInXPRAM:(BOOL)mode24
{
	uint8 byte;
	bool okay = getXPRAMat(0x8a, &byte);
	uint8 mask = ~0x05;

	if ( mode24 )
		setXPRAMat(0x8a, byte & mask);
	else
		setXPRAMat(0x8a, byte | 0x05);

	updateXPRAMWindow( dumpXPRAM() );
}

- (NSTextView*) romInfoTextView
{
	return romInfoTextView;
}

- (NSScrollView*) romInfoView
{
	return romInfoView;
}

- (void) updateROMInfoWindowWith:(char*) info
{
	[ romInfoTextView updateWithSymbols:info ];

//	[ romInfoTextView setNeedsDisplay:YES ];
	[ romInfoView setNeedsDisplay:YES ];
}

- (void) xPRAMbackup
{
	if ( uaeCreated &&
		memcmp( lastXPRAM, XPRAM, XPRAM_SIZE ) ) // if PRAM changed from copy
	{
		[ self updateXPRAMWindowWith: dumpXPRAM() ];
		memcpy ( lastXPRAM, XPRAM, XPRAM_SIZE ); // re-copy
		SaveXPRAM ();							 // and save to the file
	}
}

- (NSTextView*) XPRAMTextView
{
	return XPRAMTextView;
}

- (NSScrollView*) XPRAMScrollView
{
	return XPRAMScrollView;
}

- (void) updateXPRAMWindowWith: (char*) xpram_text
{
	[ XPRAMTextView updateWithSymbols:xpram_text ];

//	[ XPRAMTextView setNeedsDisplay:YES ];
	[ XPRAMScrollView setNeedsDisplay:YES ];
}

- (void) addressingThread
{
	if ( uaeCreated )
	{
		twentyFourBitAddressing = [ self is24bitAddrInXPRAM ];
		[ self updateAddressingButton ];
	}
}

@end
