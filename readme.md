# LivetrakPort

### Contents
- [Overview](#overview)
- [Command Line Parameters](#command-line-parameters)
- [Batch File Usage](#batch-file-usage)
- [L-12 to L-20 Migration Notes](#l-12-to-l-20-migration-notes)

## Overview

This project is a utility to port [Zoom LiveTrak L-12](https://zoomcorp.com/en/us/digital-mixer-multi-track-recorders/digital-mixer-recorder/livetrak-l-12/) projects to [Zoom LiveTrak L-20](https://zoomcorp.com/en/us/digital-mixer-multi-track-recorders/digital-mixer-recorder/livetrak-l-20/) projects.  

The Zoom LiveTrak mixers use project files that are incompatible with each other. After years of using an L-12, I wanted to switch to an L-20 and I wanted L-20 versions of all my L-12 projects without having to manually create L-20 projects or needing to import audio files on the device.

Zoom does not provide any porting utilities.  My solution started with decoding the binary project files (at least for those items that I wanted ported from L-12 to L-20 projects).  Collected notes for the `zdt` files are [available here](zdt.md).

LivetrakPort is a command line/console program that creates L-20 project files in the same directory as L-12 projects.  Since the project files are unique to each device, they are able to co-exist in the same directory structure (which is common between the L-12 and L-20).

LivetrakPort is intended to be executed via a batch file from a command-line opened to the recording media (either the root or a particular sub-directory), though it can be called directly without the use of the batch file to process one project at a time.


## Command Line Parameters

Required command-line params:
- `-t=L20TemplateProjectFile` Full path of default L-20 project file.  A default L-20 project file is included with LivetrakPort, though you can use your own.  The default project file has all channels muted by default.  If you port a full L-12 session, then channels 1-8 will be unmuted (assuming they were unmuted in the L-12 project), while channels 9-16 will be muted.  L-20 channels 17-20 will be unmuted (assuming channels 9/10 and 11/12 were unmuted in the L-12 project).
- `-s=L12sourceProjectFile` Full path of L-12 file to port to L-20

Optional command-line params:
- `-m=[rcx]` L-12 stereo track migration: (r)ename files, (c)opy files, (x)ignore files (default)
  - If `-m=c` is specified, then the L-12 track files named `TRACK09_10.WAV` and `TRACK11_12.WAV` will be copied to files named `TRACK17_18.WAV` and `TRACK19_20.WAV` for the L-20 to load.  The L-12 will continue to be able to load media for channel 9/10 and 11/12.  The copy can be time-consuming depending on your media.
  - If `-m=r` is specified, then the L-12 track files named `TRACK09_10.WAV` and `TRACK11_12.WAV` will be renamed to `TRACK17_18.WAV` and `TRACK19_20.WAV` for the L-20 to load.  The L-12 will no longer see media for channel 9/10 and 11/12.
  - If `-m=x` is specified, then the L-20 will not see any media for channels 17/18 and 19/20 unless files were already there before `x` was specified.  This option exists for cases where you want to rerun the port multiple times by deleting generated L-20 project files, and not having to deal with the stereo tracks over and over again.
- `-rX=NewName` Rename L-20 channel (where X is a value from 1 to 20) to `NewName` (maximum of 8 characters).  This is an alternative to baking channel names into the `L20TemplateProjectFile`.

Example usage:

`LivetrakPort.exe -t=d:\temp\L20default.zdt -s=f:\folder3\Song1\PRJDATA.ZDT -m=r -r1=Vox -r2=Bass -r3=Gtr1 -r4=Gtr2 -r5=DrumsL -r6=DrumsR`

## Batch File Usage

Rather than run the executable directly, use the batch file to process multiple L-12 projects in a single pass.

Before using the included batch file `portlivetrak.bat`, modify the `set` statements in the batch file.

- `LivetrakPortExe` should point to the downloaded executable
- `L20Template` should point to either the downloaded L-20 project file or your own default L-20 project file
- `RenameArgs` can be blank or you can specify channel names
- `StereoTracks` decide whether your want to copy, rename or ignore stereo media (this only affects the track media files, not the channel settings migration)

Use `del /s L20PRJDATA.ZDT` if you want to rerun the port with different arguments.
If you copied or renamed the stereo tracks in the first run, then you can use the ignore (x) option for subsequent reruns.


Example usage:	
1. `f:` assuming media card is mounted as F:
2. `cd folder01` to process all projects in the first folder
3. `yourinstallpath\portlivetrak.bat` call the batch file; it will descend into every project directory in FOLDER01

## L-12 to L-20 Migration Notes

L-12 stereo channels 9/10 and 11/12 are migrated to L-20 stereo channels 17/18 and 19/20 -- whether or not stereo track media is copied/renamed (stereo media copy/rename is controlled via the `-m` command line parameter).

L-12 EQ parameters are partially migrated to L-20 EQ parameters.  EQ Mid Freq and EQ Low Cut are not migrated.  In the generated L-20 project, those two EQ parameter values will be whatever they were in the specified L-20 template file.

L-12 effects are mapped to L-20 effect 1 or 2 as is appropriate.  Effect parameters are ported as well, except for `Room 1` and `Room 2` parameter 2.  In the generated L-20 project, Parameter 2 for `Room 1` and `Room 2` will be whatever they were in the specified L-20 template file.
