#ifndef QUICK_DISC_DOC_H
#define QUICK_DISC_DOC_H

#include "adt/ustring.h"

typedef huys::ADT::UString<char, 20> UString;

class GeneralInfo
{
public:
	UString JobType;
	UString LastRun;
	UString DiscType
	UString Priority
	int TimesRun;
	bool AllowSpanning;
};

class ProducingOptions
{
public:
	UString Bin;
	UString Line;
	bool Final;
	int Copies;
	UString Server;
	UString Cluster;
	UString HashType;
	UString MailSlot;
	UString UserData;
	UString UserType;
	UString MediaSize;
	UString MediaType;
	UString VerifyHash;
	UString EmailAddress;
	UString FinalizeType;
	UString VerifyMethod;
	int BlocksInLayer;
	bool MergeSessions;
	bool RimageProtect;
	UString VerifyChecksum; 
	bool SimulatePrinting;
	bool SimulateRecording;
	int VerifyEveryNthDisc;
};

class DDPFolders
{
public:
	UString DDPFolder1;
	UString DDPFolder2;
};

class LabelFile
{
public:
	bool Template;
	bool SaveLabel;
	bool PrintLabel;
	bool AddLabelInfo;
	bool PerfectPrint;
	UString LabelFilename;
	UString BitmapFilename;
	double PerfectPrintAngle;
};

class MergeFile
{
public:
	bool UseMerge;
	UString MergeFilename;
	bool MergeFileHasFieldNames;
};

class QuickDiscDoc
{
public:
	QuickDiscDoc();
};

#endif