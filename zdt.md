# Zoom LiveTrak Project Data File (.ZDT) Notes (for L-12 and L-20)


### Contents
- [General Notes](#general-notes)
- [L-12 PRJDATA.ZDT Project File Layout](#l-12-prjdata.zdt-project-file-layout)
- [L-20 L20PRJDATA.ZDT Project File Layout](#l-20-l20prjdata.zdt-project-file-layout)
- [Param Values](#param-values)
- [L-20 Effect 1 Types](#l-20-effect-1-types)
- [L-20 Effect 2 Types](#l-20-effect-2-types)
- [L-12 Effect Types with Mapping to L-20 Effect Type](#l-12-effect-types-with-mapping-to-l-20-effect-type)


## General Notes
- L-12 PRJDATA.ZDT: Binary file with fixed size of 4824 bytes
- L-20 L20PRJDATA.ZDT: Binary file with fixed size of 6192 bytes
- L-12 Stereo channels (9/10 and 11/12) treated like single mono channels
- L-20 Stereo channels (17/18 and 19/20) treated as explicit pair of mono channels, with most settings duplicated in both channels including channel name


Common to both:
- Nothing to note existence of recording, other then presence of wav file in same dir as project file
- a single channel recorded but not marked as playing is no different than an empty project (as far as the project file is concerned)
- recording bit depth has no impact on project file
- sample rate *IS* recorded in project file


## L-12 PRJDATA.ZDT Project File Layout
- header of 48 bytes (`ZOOM L-12    PROJECT DATA VER0002              \0`)
- channel states (mute, level, pan, ...)
    - channel data offsets
	channel 1 at 0x30
    	channel 2 at 0x60
    	channel 3 at 0x90
    	channel 4 at 0xC0
    	channel 5 at 0xF0
    	channel 6 at 0x120
    	channel 7 at 0x150
    	channel 8 at 0x180
    	channel 9/10 at 0x1B0
    	channel 11/12 at 0x1E0
	- channel data (48 bytes each channel)
    	channelBytes[0] is mute
    	channelBytes[4] is Eq on (00) or off (01)
    	channelBytes[8] is eq setting 1
    	channelBytes[16, 0x10] is eq setting 2
    	channelBytes[20, 0x14] is eq setting 3 mid freq (range 00-0x0C)
    	channelBytes[24, 0x18] is eq setting 4
    	channelBytes[32, 0x20] is pan
    	channelBytes[36, 0x24] is effect send level
    	channelBytes[40, 0x28] is eq setting 5 eq low cut on/off
    	channelBytes[44, 0x2C] is USB on (01) or off (00) (only for stereo channels 9/10 and 11/12)
- channel fader mode states at 0x210
    - fader mode data (48 bytes each mode)
    	per manual, fader modes store:
    		- Fader positions (each channel)
    		- EFX RETURN positions (1 on L-12)
    	faderBytes[0] is channel 1 level
    	faderBytes[4] is channel 2 level
    	faderBytes[8] is channel 3 level
    	faderBytes[12, 0x0C] is channel 4 level
    	faderBytes[16, 0x10] is channel 5 level
    	faderBytes[20, 0x14] is channel 6 level
    	faderBytes[24, 0x18] is channel 7 level
    	faderBytes[28, 0x1c] is channel 8 level
    	faderBytes[32, 0x20] is channel 9/10 level
    	faderBytes[36, 0x24] is channel 11/12 level
    - fader mode offsets
    	master at 0x210
    	mon A at 0x240
    	mon B at 0x270
    	mon C at 0x2A0
    	mon D at 0x2D0
    	mon E at 0x300
- master and effect states
	- master level at 0x330
	- master mute at 0x334
	- effect type at 0x338 (range 00-0x0F)
	- effect type parameters with ranges given as min - mid - max
		effect type 0 param 1 at 0x33C (range 00 - 0x32 - 0x64)
		effect type 0 param 2 at 0x340 (range 00 - 0x32 - 0x64)
		effect type 1 param 1 at 0x344 (range 00 - 0x32 - 0x64)
		effect type 1 param 2 at 0x348 (range 00 - 0x32 - 0x64)
		effect type 2 param 1 at 0x34C (range 00 - 0x32 - 0x64)
		effect type 2 param 2 at 0x350 (range 01 - 0x0F - 0x1D)
		effect type 3 param 1 at 0x354 (range 00 - 0x32 - 0x64)
		effect type 3 param 2 at 0x358 (range 00 - 0x32 - 0x64)
		effect type 4 param 1 at 0x35C (range 00 - 0x32 - 0x64)
		effect type 4 param 2 at 0x360 (range 00 - 0x32 - 0x64)
		effect type 5 param 1 at 0x364 (range 00 - 0x32 - 0x64)
		effect type 5 param 2 at 0x368 (range 00 - 0x32 - 0x64)
		effect type 6 param 1 at 0x36C (range 00 - 0x32 - 0x64)
		effect type 6 param 2 at 0x370 (range 00 - 0x32 - 0x64)
		effect type 7 param 1 at 0x374 (range 00 - 0x32 - 0x64)
		effect type 7 param 2 at 0x378 (range 00 - 0x19 - 0x32)
		effect type 8 param 1 at 0x37C (range 00 - 0x32 - 0x64)
		effect type 8 param 2 at 0x380 (range 01 - 0x0F - 0x1D)
		effect type 9 param 1 at 0x384 (range 09 - 0xF8 - 0xE7)
		effect type 9 param 1 at 0x385 (range 00 -   01 -   03)
		effect type 9 param 2 at 0x388 (range 00 - 0x32 - 0x64)
		effect type A param 1 at 0x38C (range 09 - 0xF8 - 0xE7)
		effect type A param 1 at 0x38D (range 00 -   01 -   03)
		effect type A param 2 at 0x390 (range 00 - 0x32 - 0x64)
		effect type B param 1 at 0x394 (range 09 - 0xF8 - 0xE7)
		effect type B param 1 at 0x395 (range 00 -   01 -   03)
		effect type B param 2 at 0x398 (range 00 - 0x32 - 0x64)
		effect type C param 1 at 0x39C (range 09 - 0xFF - 0xF3)
		effect type C param 1 at 0x39D (range 00 -   00 -   01)
		effect type C param 2 at 0x3A0 (range 00 - 0x32 - 0x64)
		effect type D param 1 at 0x3A4 (range 09 - 0xFE - 0xF3)
		effect type D param 1 at 0x3A5 (range 00 -   00 -   01)
		effect type D param 2 at 0x3A8 (range 00 - 0x32 - 0x64)
		effect type E param 1 at 0x3AC (range 09 - 0xFE - 0xF3)
		effect type E param 1 at 0x3AD (range 00 -   00 -   01)
		effect type E param 2 at 0x3B0 (range 00 - 0x32 - 0x64)
		effect type F param 1 at 0x3B4 (range 09 - 0xA9 - 0x49)
		effect type F param 1 at 0x3B5 (range 00 -   00 -   01)
		effect type F param 2 at 0x3B8 (range 01 - 0x0F - 0x1D)
	- effect return mute at 0x3BC
	- master effect return level at 0x3C0
	- mon A effect return level at 0x3C4
	- mon B effect return level at 0x3C8
	- mon C effect return level at 0x3CC
	- mon D effect return level at 0x3D0
	- mon E effect return level at 0x3D4
- 
- system sample rate at 0xA78
- marker count at 0xA74
-
- channel play states
	channel 1 play state (0, 1) at 0xA7C
	channel 2 play state (0, 1) at 0xA80
	channel 3 play state (0, 1) at 0xA84
	channel 4 play state (0, 1) at 0xA88
	channel 5 play state (0, 1) at 0xA8C
	channel 6 play state (0, 1) at 0xA90
	channel 7 play state (0, 1) at 0xA94
	channel 8 play state (0, 1) at 0xA98
	channel 9/10 play state (0, 1) at 0xA9C
	channel 11/12 play state (0, 1) at 0xAA0
- 
- markers at 0x05D0
	credit to: https://github.com/bananu7/livetrak-web/issues/36
	each marker is 8 bytes (64-bit), LE
	each marker is specified as number of samples from start
	empty markers are 0
	maximum number of markers is 99
	end around 0x8E8/0x8F0
- 


## L-20 L20PRJDATA.ZDT Project File Layout
- header of 48 bytes (`ZOOM L-20    PROJECT DATA VER0001              \0`)
- channel states (mute, level, pan, ...)
    - channel data offsets
		channel 1 at 0x30
		channel 2 at 0x78
		channel 3 at 0xC0
		channel 4 at 0x108
		channel 5 at 0x150
		channel 6 at 0x198
		channel 7 at 0x1E0
		channel 8 at 0x228
		channel 9 at 0x270
		channel 10 at 0x2B8
		channel 11 at 0x300
		channel 12 at 0x348
		channel 13 at 0x390
		channel 14 at 0x3D8
		channel 15 at 0x420
		channel 16 at 0x468
		channel 17 of 17/18 at 0x4B0
		channel 18 of 17/18 at 0x4F8
		channel 19 of 19/20 at 0x540
		channel 20 of 19/20 at 0x588
	- channel data (48 bytes each channel)
		solo is not retained
		polarity is not retained
		channelBytes[4] is mute
		channelBytes[8] is Eq on (00) or off (01)
		channelBytes[12, 0xC] is eq hi
		channelBytes[20, 0x14] is eq mid
		channelBytes[24, 0x18] is eq mid freq (range 00-0x60)
		channelBytes[28, 0x1C] is eq low
		channelBytes[36, 0x24] is pan (setting is not duplicated in the second of the stereo channel pairs)
		channelBytes[40, 0x28] is effect 1 send level
		channelBytes[44, 0x2C] is effect 2 send level
		channelBytes[48-52, 0x30-0x34] is eq Low Cut
		channelBytes[56, 0x38] is USB on (01) or off (00) (only for stereo channels 17/18 and 19/20)
		channelBytes[60, 0x3C] is 8 character channel name text (default to CH1\0 and CH17_18\0, not unique)
- channel fader mode states at 0x5D4
    - fader mode data (80 0x50 bytes each mode)
    	per manual, fader modes store:
    		- Fader positions (each channel)
    		- EFX RETURN positions (2 on L-12)
		faderBytes[0] is channel 1 level
		faderBytes[4] is channel 2 level
		faderBytes[8] is channel 3 level
		faderBytes[12, 0x0C] is channel 4 level
		faderBytes[16, 0x10] is channel 5 level
		faderBytes[20, 0x14] is channel 6 level
		faderBytes[24, 0x18] is channel 7 level
		faderBytes[28, 0x1c] is channel 8 level
		faderBytes[32, 0x20] is channel 9 level
		faderBytes[36, 0x24] is channel 10 level
		faderBytes[40, 0x28] is channel 11 level
		faderBytes[44, 0x2C] is channel 12 level
		faderBytes[48, 0x30] is channel 13 level
		faderBytes[52, 0x34] is channel 14 level
		faderBytes[56, 0x38] is channel 15 level
		faderBytes[60, 0x3C] is channel 16 level
		faderBytes[64, 0x40] is channel 17 of 17/18 level
		faderBytes[68, 0x44] is channel 18 of 17/18 level
		faderBytes[72, 0x48] is channel 19 of 19/20 level
		faderBytes[76, 0x4C] is channel 20 of 19/20 level
    - fader mode offsets
		master at 0x5D4
		mon A at 0x624
		mon B at 0x674
		mon C at 0x6C4
		mon D at 0x714
		mon E at 0x764
		mon F at 0x7B4
- master and effect states (level, mute, effect return, ...) at 0x804
	- master level at 0x804
	- master mute at 0x808
	- effect 1 type at 0x824 (range 00-0x09)
	- effect 1 type parameters with ranges given as min - mid - max
		effect 1 type 0 param 1 at 0x828 (range 00 - 0x32 - 0x64)
		effect 1 type 0 param 2 at 0x82C (range 00 - 0x32 - 0x64)
		effect 1 type 1 param 1 at 0x830 (range 00 - 0x32 - 0x64)
		effect 1 type 1 param 2 at 0x834 (range 00 - 0x32 - 0x64)
		effect 1 type 2 param 1 at 0x838 (range 00 - 0x32 - 0x64)
		effect 1 type 2 param 2 at 0x83C (range 00 - 0x32 - 0x64)
		effect 1 type 3 param 1 at 0x840 (range 00 - 0x32 - 0x64)
		effect 1 type 3 param 2 at 0x844 (range 00 - 0x32 - 0x64)
		effect 1 type 4 param 1 at 0x848 (range 00 - 0x32 - 0x64)
		effect 1 type 4 param 2 at 0x84C (range 00 - 0x32 - 0x64)
		effect 1 type 5 param 1 at 0x850 (range 00 - 0x32 - 0x64)
		effect 1 type 5 param 2 at 0x854 (range 00 - 0x32 - 0x64)
		effect 1 type 6 param 1 at 0x858 (range 00 - 0x32 - 0x64)
		effect 1 type 6 param 2 at 0x85C (range 00 - 0x19 - 0x32)
		effect 1 type 7 param 1 at 0x860 (range 09 - 0xFE - 0xF3)
		effect 1 type 7 param 1 at 0x861 (range 00 -   00 -   01)
		effect 1 type 7 param 2 at 0x864 (range 00 - 0x32 - 0x64)
		effect 1 type 8 param 1 at 0x868 (range 09 - 0xFE - 0xF3)
		effect 1 type 8 param 1 at 0x869 (range 00 -   00 -   01)
		effect 1 type 8 param 2 at 0x86C (range 00 - 0x32 - 0x64)
		effect 1 type 9 param 1 at 0x870 (range 09 - 0xFE - 0xF3)
		effect 1 type 9 param 1 at 0x871 (range 00 -   00 -   01)
		effect 1 type 9 param 2 at 0x874 (range 00 - 0x32 - 0x64)
		effect 1 return mute at 0x878
	- master effect 1 return level at 0x87C
	- mon A effect 1 return level at 0x880
	- mon B effect 1 return level at 0x884
	- mon C effect 1 return level at 0x888
	- mon D effect 1 return level at 0x88C
	- mon E effect 1 return level at 0x890
	- mon F effect 1 return level at 0x894
	-
	- effect 2 type at 0x8B0 (range 00-0x09)
	- effect 2 type parameters with ranges given as min - mid - max
		effect 2 type 0 param 1 at 0x8B4 (range 00 - 0x32 - 0x64)
		effect 2 type 0 param 2 at 0x8B8 (range 00 - 0x11 - 0x22)
		effect 2 type 1 param 1 at 0x8BC (range 00 - 0x32 - 0x64)
		effect 2 type 1 param 2 at 0x8C0 (range 00 - 0x0F - 0x1D)
		effect 2 type 2 param 1 at 0x8C4 (range 00 - 0x32 - 0x64)
		effect 2 type 2 param 2 at 0x8C8 (range 00 - 0x0F - 0x1D)
		effect 2 type 3 param 1 at 0x8CC (range 09 - 0xF8 - 0xE7)
		effect 2 type 3 param 1 at 0x8CD (range 00 -   01 -   03)
		effect 2 type 3 param 2 at 0x8D0 (range 00 - 0x32 - 0x64)
		effect 2 type 4 param 1 at 0x8D4 (range 09 - 0xF8 - 0xE7)
		effect 2 type 4 param 1 at 0x8D5 (range 00 -   01 -   03)
		effect 2 type 4 param 2 at 0x8D8 (range 00 - 0x32 - 0x64)
		effect 2 type 5 param 1 at 0x8DC (range 09 - 0xF8 - 0xE7)
		effect 2 type 5 param 1 at 0x8DD (range 00 -   01 -   03)
		effect 2 type 5 param 2 at 0x8E0 (range 00 - 0x32 - 0x64)
		effect 2 type 6 param 1 at 0x8E4 (range 09 - 0xA9 - 0x49)
		effect 2 type 6 param 1 at 0x8E5 (range 00 -   00 -   01)
		effect 2 type 6 param 2 at 0x8E8 (range 01 - 0x0F - 0x1D)
		effect 2 type 7 param 1 at 0x8EC (range 00 - 0x32 - 0x64)
		effect 2 type 7 param 2 at 0x8F0 (range 01 - 0x19 - 0x31)
		effect 2 type 8 param 1 at 0x8F4 (range 00 - 0x32 - 0x64)
		effect 2 type 8 param 2 at 0x8F8 (range 00 - 0x0C - 0x18)
		effect 2 type 9 param 1 at 0x8FC (range 0x31 - 0xA8 - 0x1F)
		effect 2 type 9 param 1 at 0x8FD (range   00 -   01 -   03)
		effect 2 type 9 param 2 at 0x900 (range   01 - 0x19 - 0x31)
	- effect 2 return mute at 0x904
	- master effect 2 return level at 0x908
	- mon A effect 2 return level at 0x90C
	- mon B effect 2 return level at 0x910
	- mon C effect 2 return level at 0x914
	- mon D effect 2 return level at 0x918
	- mon E effect 2 return level at 0x91C
	- mon F effect 2 return level at 0x920
- 
- markers at 0xB40
	credit to: https://github.com/bananu7/livetrak-web/issues/36
	each marker is 8 bytes (64-bit), LE
	each marker is specified as number of samples from start
	empty markers are 0
	maximum number of markers is 99
	end around 0xCCC
-
- system sample rate at 0xFE8
- marker count at 0xFE4
-
- channel play states
  	channel 1 play state (0, 1) at 0xFE9
	channel 2 play state (0, 1) at 0xFEA
	channel 3 play state (0, 1) at 0xFEB
	channel 4 play state (0, 1) at 0xFEC
	channel 5 play state (0, 1) at 0xFED
	channel 6 play state (0, 1) at 0xFEE
	channel 7 play state (0, 1) at 0xFEF
	channel 8 play state (0, 1) at 0xFF0
	channel 9 play state (0, 1) at 0xFF1
	channel 10 play state (0, 1) at 0xFF2
	channel 11 play state (0, 1) at 0xFF3
	channel 12 play state (0, 1) at 0xFF4
	channel 13 play state (0, 1) at 0xFF5
	channel 14 play state (0, 1) at 0xFF6
	channel 15 play state (0, 1) at 0xFF7
	channel 16 play state (0, 1) at 0xFF8
	channel 17 play state (0, 1) at 0xFF9
	channel 18 play state (0, 1) at 0xFFA
	channel 19 play state (0, 1) at 0xFFB
	channel 20 play state (0, 1) at 0xFFC
- 


## Param Values
mute off => 00
mute on => 01

pan -100 (left) => 00
pan 0 (middle) => 0x30
pan +100 (right) => 0x60

eq settings min => 00
eq settings mid => 0x1E
eq settings max => 0x3C
L-12 eq mid freq range 00-0x0C
L-20 eq mid freq range 00-0x60
effect send level max => 0x3C
L-12 effect type range 00-0x0F
L-20 fx1 effect type range 00-0x09
L-20 fx2 effect type range 00-0x09

level max => 0x7F
level mid => 0x3F
level min => 00

system sample rate: 0=44, 1=48, ?=96


## L-20 Effect 1 Types
    0: Hall 1
    1: Hall 2
    2: Room 1
    3: Plate
    4: Church
    5: DrumAmb
    6: GateRev
    7: Vocal 1
    8: Vocal 2
    9: Vocal 3

## L-20 Effect 2 Types
    0: Hall 3
    1: Room 2
    2: Spring
    3: Delay
    4: Analog
    5: P-P Dly
    6: Vocal 4
    7: Chorus 1
    8: Chorus 2
    9: Chorus+Delay

## L-12 Effect Types with Mapping to L-20 Effect Type
    0: Hall 1   => L-20 fx1 0
    1: Hall 2   => L-20 fx1 1
    2: Room 1   => L-20 fx1 2   (param 2 does not map directly)
    3: Room 2   => L-20 fx2 1   (param 2 does not map directly)
    4: Plate    => L-20 fx1 3
    5: Church   => L-20 fx1 4
    6: DrumAmb  => L-20 fx1 5
    7: GateRev  => L-20 fx1 6
    8: Spring   => L-20 fx2 2
    9: Delay    => L-20 fx2 3
    A: Analog   => L-20 fx2 4
    B: P-P Dly  => L-20 fx2 5
    C: Vocal 1  => L-20 fx1 7
    D: Vocal 2  => L-20 fx1 8
    E: Vocal 3  => L-20 fx1 9
    F: Vocal 4  => L-20 fx2 6
