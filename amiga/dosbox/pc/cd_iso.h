/* GPL-2.0-or-later. DOSBox ISO drive declarations. */
#pragma once
#include "drive.h"
#include "drive_decl.h"
#include "cross.h"
#undef MAX_OPENDIRS
#define MAX_OPENDIRS 16
struct isoPVD {
	Bit8u type;
	Bit8u standardIdent[5];
	Bit8u version;
	Bit8u unused1;
	Bit8u systemIdent[32];
	Bit8u volumeIdent[32];
	Bit8u unused2[8];
	Bit32u volumeSpaceSizeL;
	Bit32u volumeSpaceSizeM;
	Bit8u unused3[32];
	Bit16u volumeSetSizeL;
	Bit16u volumeSetSizeM;
	Bit16u volumeSeqNumberL;
	Bit16u volumeSeqNumberM;
	Bit16u logicBlockSizeL;
	Bit16u logicBlockSizeM;
	Bit32u pathTableSizeL;
	Bit32u pathTableSizeM;
	Bit32u locationPathTableL;
	Bit32u locationOptPathTableL;
	Bit32u locationPathTableM;
	Bit32u locationOptPathTableM;
	Bit8u rootEntry[34];
	Bit32u unused4[1858];
} GCC_ATTRIBUTE(packed);

struct isoDirEntry {
	Bit8u length;
	Bit8u extAttrLength;
	Bit32u extentLocationL;
	Bit32u extentLocationM;
	Bit32u dataLengthL;
	Bit32u dataLengthM;
	Bit8u dateYear;
	Bit8u dateMonth;
	Bit8u dateDay;
	Bit8u timeHour;
	Bit8u timeMin;
	Bit8u timeSec;
	Bit8u timeZone;
	Bit8u fileFlags;
	Bit8u fileUnitSize;
	Bit8u interleaveGapSize;
	Bit16u VolumeSeqNumberL;
	Bit16u VolumeSeqNumberM;
	Bit8u fileIdentLength;
	Bit8u ident[222];
} GCC_ATTRIBUTE(packed);

#ifdef _MSC_VER
#pragma pack ()
#endif

#if defined (WORDS_BIGENDIAN)
#define EXTENT_LOCATION(de)	((de).extentLocationM)
#define DATA_LENGTH(de)		((de).dataLengthM)
#else
#define EXTENT_LOCATION(de)	((de).extentLocationL)
#define DATA_LENGTH(de)		((de).dataLengthL)
#endif

#define ISO_FRAMESIZE		2048
#define ISO_ASSOCIATED		4
#define ISO_DIRECTORY		2
#define ISO_HIDDEN		1
#define ISO_MAX_FILENAME_LENGTH 37
#define ISO_MAXPATHNAME		256
#define ISO_FIRST_VD		16
#define IS_ASSOC(fileFlags)	(fileFlags & ISO_ASSOCIATED)
#define IS_DIR(fileFlags)	(fileFlags & ISO_DIRECTORY)
#define IS_HIDDEN(fileFlags)	(fileFlags & ISO_HIDDEN)
#define ISO_MAX_HASH_TABLE_SIZE 	100

class isoDrive : public DOS_Drive {
public:
	isoDrive(char driveLetter, const char* device_name, Bit8u mediaid, int &error);
	~isoDrive();
	virtual bool FileOpen(DOS_File **file, char *name, Bit32u flags);
	virtual bool FileCreate(DOS_File **file, char *name, Bit16u attributes);
	virtual bool FileUnlink(char *name);
	virtual bool RemoveDir(char *dir);
	virtual bool MakeDir(char *dir);
	virtual bool TestDir(char *dir);
	virtual bool FindFirst(char *_dir, DOS_DTA &dta, bool fcb_findfirst);
	virtual bool FindNext(DOS_DTA &dta);
	virtual bool GetFileAttr(char *name, Bit16u *attr);
	virtual bool Rename(char * oldname,char * newname);
	virtual bool AllocationInfo(Bit16u *bytes_sector, Bit8u *sectors_cluster, Bit16u *total_clusters, Bit16u *free_clusters);
	virtual bool FileExists(const char *name);
   	virtual bool FileStat(const char *name, FileStat_Block *const stat_block);
	virtual Bit8u GetMediaByte(void);
	virtual void EmptyCache(void){}
	virtual bool isRemote(void);
	virtual bool isRemovable(void);
	virtual Bits UnMount(void);
	bool readSector(Bit8u *buffer, Bit32u sector);
	virtual char const* GetLabel(void) {return discLabel;};
	virtual void Activate(void);
private:
	int  readDirEntry(isoDirEntry *de, Bit8u *data);
	bool loadImage();
	bool lookupSingle(isoDirEntry *de, const char *name, Bit32u sectorStart, Bit32u length);
	bool lookup(isoDirEntry *de, const char *path);
	int  UpdateMscdex(char driveLetter, const char* physicalPath, Bit8u& subUnit);
	int  GetDirIterator(const isoDirEntry* de);
	bool GetNextDirEntry(const int dirIterator, isoDirEntry* de);
	void FreeDirIterator(const int dirIterator);
	bool ReadCachedSector(Bit8u** buffer, const Bit32u sector);
	
	struct DirIterator {
		bool valid;
		bool root;
		Bit32u currentSector;
		Bit32u endSector;
		Bit32u pos;
	} dirIterators[MAX_OPENDIRS];
	
	int nextFreeDirIterator;
	
	struct SectorHashEntry {
		bool valid;
		Bit32u sector;
		Bit8u data[ISO_FRAMESIZE];
	} sectorHashEntries[ISO_MAX_HASH_TABLE_SIZE];

	bool iso;
	bool dataCD;
	isoDirEntry rootEntry;
	Bit8u mediaid;
	char fileName[CROSS_LEN];
	Bit8u subUnit;
	char driveLetter;
	char discLabel[32];
};

