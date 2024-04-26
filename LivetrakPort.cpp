/*
 * LivetrakPort
 * License: UNLICENSE
 *
 * Let me know if you modify, extend or use this.
 * My website: http://www.creepingfog.com/sean/
 * Contact Sean: "fester" at the domain of my website
 */

#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>


using byte = unsigned char;
using Bytes = std::vector<byte>;
namespace fs = std::filesystem;
using ChannelNameMapping = std::map<int, std::string>;


void DisplayHelp();
Bytes LoadTemplateFile(std::wstring filename);
Bytes LoadSourceFile(std::wstring sourceFilename);
void PortL12ToL20(const Bytes &srcData, Bytes &targetData);
void RenameL20Channels(Bytes &targetData, const ChannelNameMapping &names);
fs::path BuildandTestTargetFilepath(std::wstring sourceFilename);
void CreateFileFromBytes(fs::path targetFilepath, const Bytes &data);
void CopyL12StereoTracks(std::wstring sourceFilename);
void RenameL12StereoTracks(std::wstring sourceFilename);


struct AppParams
{
	std::wstring mTemplateFilename, mSourceFilename;
	ChannelNameMapping mChannelNames;
	bool mRenameStereoTracks = false;
	bool mCopyStereoTracks = false;

	void ParseArgs(int argc, wchar_t* argv[]);
};


int
wmain(int argc, wchar_t* argv[])
{
	AppParams appParams;

	try
	{
		appParams.ParseArgs(argc, argv);
	}
	catch (std::exception &ex)
	{
		printf("command line parameter error: %s\n", ex.what());
		DisplayHelp();
		return 0;
	}

	try
	{
		printf("processing \"%ws\" with template file \"%ws\"...\n", appParams.mSourceFilename.c_str(), appParams.mTemplateFilename.c_str());
		Bytes l20data{ LoadTemplateFile(appParams.mTemplateFilename) };
		const Bytes l12data{ LoadSourceFile(appParams.mSourceFilename) };
		const fs::path tgtFilepath{ BuildandTestTargetFilepath(appParams.mSourceFilename) };
		PortL12ToL20(l12data, l20data);
		RenameL20Channels(l20data, appParams.mChannelNames);
		CreateFileFromBytes(tgtFilepath, l20data);

		// The L-12 stereo tracks need to be relocated to different channels on the L-20
		if (appParams.mRenameStereoTracks)
			RenameL12StereoTracks(appParams.mSourceFilename);
		else if (appParams.mCopyStereoTracks)
			CopyL12StereoTracks(appParams.mSourceFilename);

		return 1;
	}
	catch (std::exception &ex)
	{
		printf("an error occurred: %s\n", ex.what());
		return -1;
	}

	return 0;
}

void
DisplayHelp()
{
	printf("\nUse LivetrakPort to migrate Zoom LiveTrak L-12 projects to L-20.\n"
		"Usage:\n"
		"LivetrakPort -t=L20templateFile -s=L12sourceProjectFile [-m=[one of: rcx]] [-rX=NewName [where X=1-20]]\n"
		"    -t=L20templateFile       : Full path of default L-20 project file\n"
		"    -s=L12sourceProjectFile  : Full path of L-12 file to port to L-20\n"
		"    -m=[rcx]                 : migrate stereo track files: (r)ename, (c)opy, (x)ignore (default)\n"
		"    -r1=Vox                  : rename L-20 channel 1 to Vox (max of 8 characters)\n"
	);
}

void
ValidateChannelName(const std::string name)
{
	if (name.length() > 8)
		throw std::exception("Channel name is too long");

	constexpr char validChars[] = " !#$%&‘()+,-0123456789;=@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{~}";
	auto invalidChar = name.find_first_not_of(validChars);
	if (invalidChar != -1)
		throw std::exception("Channel name contains illegal character");
}

void
AppParams::ParseArgs(int argc, wchar_t* argv[])
{
	// #consider command line args to remap L-12 channels 1-8 to other L-20 channels? (L-12 9-12 are already automatically remapped to L-20 17-20))

	std::vector<std::wstring> args;
	// prepare args
	for (int i = 1; i < argc; ++i)
	{
		std::wstring curArg(argv[i]);
		if (curArg.empty())
			throw std::exception("invalid argument");

		if (curArg[0] != '-')
		{
			// assume any arg that doesn't start with '-' is part of previous param (filename with space in path)
			if (args.empty())
				throw std::exception("invalid argument");

			std::vector<std::wstring>::iterator it = args.end();
			std::wstring prevArg(*--it);
			prevArg += L" ";
			prevArg += curArg;
			curArg = prevArg;
			args.pop_back();
		}

		if (curArg.length() < 2)
		{
			// assume all command line params are minimum length of 2 including leading '-'
			throw std::exception("invalid argument");
		}

		args.emplace_back(curArg);
	}

	for (const auto &tmp : args)
	{
		switch (tmp[1])
		{
		case 't':
			// L-20 template project: -t=file
			if (tmp.length() < 3 || tmp[2] != '=')
				throw std::exception("bad t argument");
			mTemplateFilename = &tmp[3];
			break;
		case 's':
			// L-12 source project: -s=file
			if (tmp.length() < 3 || tmp[2] != '=')
				throw std::exception("bad s argument");
			mSourceFilename = &tmp[3];
			break;
		case 'm': 
			// stereo track migration: -m=[rcx]
			if (tmp.length() != 4 || tmp[2] != '=')
				throw std::exception("bad m argument");
			if (tmp[3] == 'r')
				mRenameStereoTracks = true;
			else if (tmp[3] == 'c')
				mCopyStereoTracks = true;
			else if (tmp[3] == 'x')
				;
			else
				throw std::exception("bad m argument");
			break;
		case 'r':
			// rename channels:	-rX=foo -rXX=foo
			if (tmp.length() > 5 && (tmp[3] == '=' || tmp[4] == '='))
			{
				int eqIdx = -1;
				int ch = -1;

				if (tmp[3] == '=')
				{
					// ch 1-9
					eqIdx = 4;
					ch = tmp[2] - 0x30;
				}
				else if (tmp[4] == '=' && (tmp[2] == '1' || (tmp[2] == '2' && tmp[3] == '0')))
				{
					// ch 10-20
					eqIdx = 5;
					if (tmp[2] == '1')
					{
						// ch 10-19
						ch = tmp[3] - 0x30;
						ch += 10;
					}
					else
						ch = 20;
				}

				if (eqIdx != -1 && ch > 0 && ch < 21)
				{
					const std::wstring wName(&tmp[eqIdx]);
#pragma warning(push)
#pragma warning(disable: 4244)
					// Zoom LiveTrak names support limited character set
					const std::string name(wName.begin(), wName.end());
#pragma warning(pop)
					ValidateChannelName(name);
					mChannelNames[ch] = name;
				}
				else
					throw std::exception("bad channel rename parameter");
			}
			else
				throw std::exception("bad r argument");
			break;
		default:
			throw std::exception("Unhandled flag");
		}
	}

	if (mTemplateFilename.empty())
		throw std::exception("No L-20 template file specified");

	if (mSourceFilename.empty())
		throw std::exception("No L-12 project file specified (nothing to port?)");
}

Bytes
LoadTemplateFile(std::wstring templateFilename)
{
	if (templateFilename.empty())
		throw std::exception("No template file specified.");

	fs::path tmplFilepath(templateFilename);
	if (!fs::exists(tmplFilepath))
		throw std::exception("Specified template file could not be located.");

	const uintmax_t sz = fs::file_size(tmplFilepath);
	constexpr uintmax_t kL20Filesize = 6192;
	if (kL20Filesize != sz)
		throw std::exception("Size of specified template file is unexpected.");

	Bytes data(sz);

	{
		std::ifstream inputFilestream(templateFilename, std::ios::binary);
		if (!inputFilestream.read(reinterpret_cast<char*>(data.data()), sz))
			throw std::exception("Unexpected read error of specified template file.");

		inputFilestream.close();
	}

	const byte L20Header[]{"ZOOM L-20    PROJECT DATA VER0001              "};
	if (::memcmp(L20Header, &data[0], sizeof(L20Header)))
		throw std::logic_error("Unexpected project data version info in specified template file.");

	return data;
}

Bytes
LoadSourceFile(std::wstring sourceFilename)
{
	if (sourceFilename.empty())
		throw std::exception("No source file specified.");

	fs::path srcFilepath(sourceFilename);
	if (!fs::exists(srcFilepath))
		throw std::exception("Specified source file could not be located.");

	const uintmax_t sz = fs::file_size(srcFilepath);
	constexpr uintmax_t kL12Filesize = 4824;
	if (kL12Filesize != sz)
		throw std::exception("Size of specified source file is unexpected.");

	Bytes data(sz);

	{
		std::ifstream inputFilestream(sourceFilename, std::ios::binary);
		if (!inputFilestream.read(reinterpret_cast<char*>(data.data()), sz))
			throw std::exception("Unexpected read error of specified source file.");

		inputFilestream.close();
	}

	const byte L12Header[]{"ZOOM L-12    PROJECT DATA VER0002              "};
	if (::memcmp(L12Header, &data[0], sizeof(L12Header)))
		throw std::logic_error("Unexpected project data version info in specified source file.");

	return data;
}

void
RenameL20Channels(Bytes &targetData, const ChannelNameMapping &names)
{
	if (names.empty())
		return;

	printf("renaming channels...\n");
	using OffsetMapping = std::map<int, int>; // channel number (base 1) to offset for name chars in the channel data
	OffsetMapping offsets;
	constexpr int kNameOffsetInChannelData = 0x3C;
	offsets[1] = 0x30 + kNameOffsetInChannelData;
	offsets[2] = 0x78 + kNameOffsetInChannelData;
	offsets[3] = 0xC0 + kNameOffsetInChannelData;
	offsets[4] = 0x108 + kNameOffsetInChannelData;
	offsets[5] = 0x150 + kNameOffsetInChannelData;
	offsets[6] = 0x198 + kNameOffsetInChannelData;
	offsets[7] = 0x1E0 + kNameOffsetInChannelData;
	offsets[8] = 0x228 + kNameOffsetInChannelData;
	offsets[9] = 0x270 + kNameOffsetInChannelData;
	offsets[10] = 0x2B8 + kNameOffsetInChannelData;
	offsets[11] = 0x300 + kNameOffsetInChannelData;
	offsets[12] = 0x348 + kNameOffsetInChannelData;
	offsets[13] = 0x390 + kNameOffsetInChannelData;
	offsets[14] = 0x3D8 + kNameOffsetInChannelData;
	offsets[15] = 0x420 + kNameOffsetInChannelData;
	offsets[16] = 0x468 + kNameOffsetInChannelData;
	offsets[17] = 0x4B0 + kNameOffsetInChannelData;
	offsets[18] = 0x4F8 + kNameOffsetInChannelData;
	offsets[19] = 0x540 + kNameOffsetInChannelData;
	offsets[20] = 0x588 + kNameOffsetInChannelData;

	for (const auto &it : names)
	{
		if (it.second.length() > 8)
			throw std::logic_error("Channel rename operation: new name is too long");

		// overwrite old name with nulls in case old name is longer than new name
		memset(&targetData[offsets[it.first]], 0, 8);

		memcpy(&targetData[offsets[it.first]], it.second.c_str(), it.second.length());
	}
}

fs::path
BuildandTestTargetFilepath(std::wstring sourceFilename)
{
	fs::path targetFilepath(sourceFilename);
	if (targetFilepath.empty())
		throw std::exception("No target file specified.");

	targetFilepath.replace_filename("L20PRJDATA.ZDT");

	if (fs::exists(targetFilepath))
		throw std::exception("The target file already exists -- either rename or delete L20PRJDATA.ZDT.");

	return targetFilepath;
}

void
CreateFileFromBytes(fs::path targetFilepath, const Bytes &data)
{
	printf("creating file: %ws...\n", targetFilepath.c_str());
	std::ofstream outputFilestream(targetFilepath, std::ios::binary | std::ios::out);
	if (!outputFilestream.write((char*)data.data(), data.size()))
		throw std::exception("Unexpected error writing to target file.");

	outputFilestream.close();
}

void
CopyFile(fs::path src, fs::path tgt)
{
	if (fs::exists(src))
	{
		if (fs::exists(tgt))
		{
			if (fs::file_size(src) == fs::file_size(tgt))
				printf("warn: attempt to copy L-12 track to L-20 aborted due to a file with the same name and size already exists: %ws\n", tgt.c_str());
			else
				printf("warn: attempt to copy L-12 track to L-20 aborted due to a file with the same name but different size already exists: %ws\n", tgt.c_str());
		}
		else
		{
			printf("copying file \"%ws\" to \"%ws\"...\n", src.c_str(), tgt.c_str());
			fs::copy_file(src, tgt);
		}
	}
}

void
CopyL12StereoTracks(std::wstring sourceFilename)
{
	fs::path src(sourceFilename);
	fs::path tgt(sourceFilename);

	src.replace_filename("TRACK09_10.WAV");
	tgt.replace_filename("TRACK17_18.WAV");
	CopyFile(src, tgt);

	src.replace_filename("TRACK11_12.WAV");
	tgt.replace_filename("TRACK19_20.WAV");
	CopyFile(src, tgt);
}

void
RenameFile(fs::path src, fs::path tgt)
{
	if (fs::exists(src))
	{
		if (fs::exists(tgt))
		{
			if (fs::file_size(src) == fs::file_size(tgt))
				printf("warn: attempt to rename L-12 track to L-20 aborted due to a file with the same name and size already exists: %ws\n", tgt.c_str());
			else
				printf("warn: attempt to rename L-12 track to L-20 aborted due to a file with the same name but different size already exists: %ws\n", tgt.c_str());
		}
		else
		{
			printf("renaming file \"%ws\" to \"%ws\"...\n", src.c_str(), tgt.c_str());
			fs::rename(src, tgt);
		}
	}
}

void
RenameL12StereoTracks(std::wstring sourceFilename)
{
	fs::path src(sourceFilename);
	fs::path tgt(sourceFilename);

	src.replace_filename("TRACK09_10.WAV");
	tgt.replace_filename("TRACK17_18.WAV");
	RenameFile(src, tgt);

	src.replace_filename("TRACK11_12.WAV");
	tgt.replace_filename("TRACK19_20.WAV");
	RenameFile(src, tgt);
}

void
PortL12ChannelState(const byte *srcData, byte *targetData, bool fx1 = true, bool doPan = true)
{
	// mute
	targetData[4] = srcData[0];

	// eq on/off
	targetData[8] = srcData[4];

	// eq
	targetData[0xC] = srcData[8]; // hi
	targetData[0x14] = srcData[0x10]; // mid
	// targetData[0x18] = srcData[0x14]; mid freq, mid freq range does not map directly #valueMappingMismatch
	targetData[0x1C] = srcData[0x18]; // low
	// eq low cut does not map directly #valueMappingMismatch
	// srcData[0x28] is eq low cut on / off
	// targetData[48-52, 0x30-0x34] is eq Low Cut

	//  pan (not duplicated in both channels of a stereo pair)
	if (doPan)
		targetData[0x24] = srcData[0x20];

	// effect send level
	targetData[fx1 ? 0x28 : 0x2C] = srcData[0x24];

	// usb
	targetData[0x38] = srcData[0x2C];
}

void
PortL12FaderModes(const byte *srcData, byte *targetData)
{
	targetData[0] = srcData[0]; // ch 1
	targetData[4] = srcData[4]; // ch 2
	targetData[8] = srcData[8]; // ch 3
	targetData[0x0C] = srcData[0x0C]; // ch 4
	targetData[0x10] = srcData[0x10]; // ch 5
	targetData[0x14] = srcData[0x14]; // ch 6
	targetData[0x18] = srcData[0x18]; // ch 7
	targetData[0x1c] = srcData[0x1c]; // ch 8
	targetData[0x40] = srcData[0x20]; // L-20 ch 17 of 17/18, L-12 9/10
	targetData[0x44] = srcData[0x20]; // L-20 ch 18 of 17/18, L-12 9/10
	targetData[0x48] = srcData[0x24]; // L-20 ch 19 of 19/20, L-12 11/12
	targetData[0x4C] = srcData[0x24]; // L-20 ch 20 of 19/20, L-12 11/12
}

bool
L12EffectUsesL20Effect1(byte l12EffectType)
{
	// see ZDT.md section "L-12 Effect Types with Mapping to L-20 Effect Type"
	switch (l12EffectType)
	{
	case 3:
	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
		return false;
	}
	return true;
}

byte
MapL12EffectToL20Effect(byte l12EffectType)
{
	// see ZDT.md section "L-12 Effect Types with Mapping to L-20 Effect Type"
	switch (l12EffectType)
	{
	case 0:	return 0;
	case 1: return 1;
	case 2: return 2;
	case 3: return 1;
	case 4: return 3;
	case 5: return 4;
	case 6: return 5;
	case 7: return 6;
	case 8: return 2;
	case 9: return 3;
	case 10: return 4;
	case 11: return 5;
	case 12: return 7;
	case 13: return 8;
	case 14: return 9;
	case 15: return 6;
	}

	throw std::exception("Unexpected L-12 effect type.");
}

void
PortL12EffectParams(const Bytes &srcData, Bytes &targetData)
{
	// L-12 fx 0
	targetData[0x828] = srcData[0x33C];
	targetData[0x82C] = srcData[0x340];
	// L-12 fx 1
	targetData[0x830] = srcData[0x344];
	targetData[0x834] = srcData[0x348];
	// L-12 fx 2
	// (2) Room 1 -> fx1 2; param 2 does not map direct, leave param 2 at default in L-20 proj #valueMappingMismatch
	targetData[0x838] = srcData[0x34C];
	// L-12 fx 3
	// (3) Room 2 -> fx2 1; param 2 does not map direct, leave param 2 at default in L-20 proj #valueMappingMismatch
	targetData[0x8BC] = srcData[0x354];
	// L-12 fx 4
	targetData[0x840] = srcData[0x35C];
	targetData[0x844] = srcData[0x360];
	// L-12 fx 5
	targetData[0x848] = srcData[0x364];
	targetData[0x84C] = srcData[0x368];
	// L-12 fx 6
	targetData[0x850] = srcData[0x36C];
	targetData[0x854] = srcData[0x370];
	// L-12 fx 7
	targetData[0x858] = srcData[0x374];
	targetData[0x85C] = srcData[0x378];
	// L-12 fx 8
	targetData[0x8C4] = srcData[0x37C];
	targetData[0x8C8] = srcData[0x380];
	// L-12 fx 9
	targetData[0x8CC] = srcData[0x384];
	targetData[0x8CD] = srcData[0x385];
	targetData[0x8D0] = srcData[0x388];
	// L-12 fx 10
	targetData[0x8D4] = srcData[0x38C];
	targetData[0x8D5] = srcData[0x38D];
	targetData[0x8D8] = srcData[0x390];
	// L-12 fx 11
	targetData[0x8DC] = srcData[0x394];
	targetData[0x8DD] = srcData[0x395];
	targetData[0x8E0] = srcData[0x398];
	// L-12 fx 12
	targetData[0x860] = srcData[0x39C];
	targetData[0x861] = srcData[0x39D];
	targetData[0x864] = srcData[0x3A0];
	// L-12 fx 13
	targetData[0x868] = srcData[0x3A4];
	targetData[0x869] = srcData[0x3A5];
	targetData[0x86C] = srcData[0x3A8];
	// L-12 fx 14
	targetData[0x870] = srcData[0x3AC];
	targetData[0x871] = srcData[0x3AD];
	targetData[0x874] = srcData[0x3B0];
	// L-12 fx 15
	targetData[0x8E4] = srcData[0x3B4];
	targetData[0x8E5] = srcData[0x3B5];
	targetData[0x8E8] = srcData[0x3B8];
}

void
PortL12ChannelPlayStates(Bytes &targetData, const Bytes & srcData)
{
	targetData[0xFE9] = srcData[0xA7C]; // ch 1
	targetData[0xFEA] = srcData[0xA80]; // ch 2
	targetData[0xFEB] = srcData[0xA84]; // ch 3
	targetData[0xFEC] = srcData[0xA88]; // ch 4
	targetData[0xFED] = srcData[0xA8C]; // ch 5
	targetData[0xFEE] = srcData[0xA90]; // ch 6
	targetData[0xFEF] = srcData[0xA94]; // ch 7
	targetData[0xFF0] = srcData[0xA98]; // ch 8
	targetData[0xFF9] = srcData[0xA9C]; // L-12 ch 9/10, L-20 ch 17
	targetData[0xFFA] = srcData[0xA9C]; // L-12 ch 9/10, L-20 ch 18
	targetData[0xFFB] = srcData[0xAA0]; // L-12 ch 11/12, L-20 ch 19
	targetData[0xFFC] = srcData[0xAA0]; // L-12 ch 11/12, L-20 ch 20
}

void
PortL12EffectLevels(Bytes &targetData, bool useL20Fx1, const Bytes &srcData)
{
	targetData[useL20Fx1 ? 0x878 : 0x904] = srcData[0x3BC]; // effect return mute
	targetData[useL20Fx1 ? 0x87C : 0x908] = srcData[0x3C0]; // master effect return level
	targetData[useL20Fx1 ? 0x880 : 0x90C] = srcData[0x3C4]; // mon A effect return level
	targetData[useL20Fx1 ? 0x884 : 0x910] = srcData[0x3C8]; // mon B effect return level
	targetData[useL20Fx1 ? 0x888 : 0x914] = srcData[0x3CC]; // mon C effect return level
	targetData[useL20Fx1 ? 0x88C : 0x918] = srcData[0x3D0]; // mon D effect return level
	targetData[useL20Fx1 ? 0x890 : 0x91C] = srcData[0x3D4]; // mon E effect return level
}

void
PortL12ToL20(const Bytes &srcData, Bytes &targetData)
{
	const byte l12EffectType = srcData[0x338];
	const bool useL20Fx1 = L12EffectUsesL20Effect1(l12EffectType);

	PortL12ChannelState(&srcData[0x30], &targetData[0x30], useL20Fx1); // ch 1
	PortL12ChannelState(&srcData[0x60], &targetData[0x78], useL20Fx1); // ch 2
	PortL12ChannelState(&srcData[0x90], &targetData[0xC0], useL20Fx1); // ch 3
	PortL12ChannelState(&srcData[0xC0], &targetData[0x108], useL20Fx1); // ch 4
	PortL12ChannelState(&srcData[0xF0], &targetData[0x150], useL20Fx1); // ch 5
	PortL12ChannelState(&srcData[0x120], &targetData[0x198], useL20Fx1); // ch 6
	PortL12ChannelState(&srcData[0x150], &targetData[0x1E0], useL20Fx1); // ch 7
	PortL12ChannelState(&srcData[0x180], &targetData[0x228], useL20Fx1); // ch 8
	PortL12ChannelState(&srcData[0x1B0], &targetData[0x4B0], useL20Fx1); // L-12 ch 9 to L-20 17
	PortL12ChannelState(&srcData[0x1B0], &targetData[0x4F8], useL20Fx1, false); // L-12 ch 10 to L-20 18
	PortL12ChannelState(&srcData[0x1E0], &targetData[0x540], useL20Fx1); // L-12 ch 11 to L-20 19
	PortL12ChannelState(&srcData[0x1E0], &targetData[0x588], useL20Fx1, false); // L-12 ch 12 to L-20 20

	PortL12FaderModes(&srcData[0x210], &targetData[0x5D4]); // master
	PortL12FaderModes(&srcData[0x240], &targetData[0x624]); // mon A
	PortL12FaderModes(&srcData[0x270], &targetData[0x674]); // mon B
	PortL12FaderModes(&srcData[0x2A0], &targetData[0x6C4]); // mon C
	PortL12FaderModes(&srcData[0x2D0], &targetData[0x714]); // mon D
	PortL12FaderModes(&srcData[0x300], &targetData[0x764]); // mon E

	// master level
	targetData[0x804] = srcData[0x330];
	// master mute
	targetData[0x808] = srcData[0x334];

	// effect type (single one from L-12, maps to either fx1 or fx2 on L-20)
	targetData[useL20Fx1 ? 0x824 : 0x8B0] = MapL12EffectToL20Effect(l12EffectType);
	PortL12EffectParams(srcData, targetData);
	PortL12EffectLevels(targetData, useL20Fx1, srcData);

	// marker count
	targetData[0xFE4] = srcData[0xA74];
	// sample rate
	targetData[0xFE8] = srcData[0xA78];

	PortL12ChannelPlayStates(targetData, srcData);

	// markers
	// credit to: https://github.com/bananu7/livetrak-web/issues/36
	// 	each marker is 8 bytes (64-bit), LE
	// 	each marker is specified as number of samples from start
	// 	empty markers are 0
	constexpr size_t kMaxMarkers = 99;
	constexpr size_t kMarkerSize = 8;
	memcpy(&targetData[0xB40], &srcData[0x05D0], kMaxMarkers * kMarkerSize);
}
