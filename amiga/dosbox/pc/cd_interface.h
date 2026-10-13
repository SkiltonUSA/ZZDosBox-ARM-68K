/* GPL-2.0-or-later. DOSBox interface; read-only XACP transport backend. */
#pragma once
#include "dosbox.h"
#include "mem.h"
#define RAW_SECTOR_SIZE		2352
#define COOKED_SECTOR_SIZE	2048

enum { CDROM_USE_SDL, CDROM_USE_ASPI, CDROM_USE_IOCTL_DIO, CDROM_USE_IOCTL_DX, CDROM_USE_IOCTL_MCI };

typedef struct SMSF {
	unsigned char min;
	unsigned char sec;
	unsigned char fr;
} TMSF;

typedef struct SCtrl {
	Bit8u	out[4];			// output channel
	Bit8u	vol[4];			// channel volume
} TCtrl;

extern int CDROM_GetMountType(char* path, int force);

class CDROM_Interface
{
public:
//	CDROM_Interface						(void);
	virtual ~CDROM_Interface			(void) {};

	virtual bool	SetDevice			(char* path, int forceCD) = 0;

	virtual bool	GetUPC				(unsigned char& attr, char* upc) = 0;

	virtual bool	GetAudioTracks		(int& stTrack, int& end, TMSF& leadOut) = 0;
	virtual bool	GetAudioTrackInfo	(int track, TMSF& start, unsigned char& attr) = 0;
	virtual bool	GetAudioSub			(unsigned char& attr, unsigned char& track, unsigned char& index, TMSF& relPos, TMSF& absPos) = 0;
	virtual bool	GetAudioStatus		(bool& playing, bool& pause) = 0;
	virtual bool	GetMediaTrayStatus	(bool& mediaPresent, bool& mediaChanged, bool& trayOpen) = 0;

	virtual bool	PlayAudioSector		(unsigned long start,unsigned long len) = 0;
	virtual bool	PauseAudio			(bool resume) = 0;
	virtual bool	StopAudio			(void) = 0;
	virtual void	ChannelControl		(TCtrl ctrl) = 0;
	
	virtual bool	ReadSectors			(PhysPt buffer, bool raw, unsigned long sector, unsigned long num) = 0;

	virtual bool	LoadUnloadMedia		(bool unload) = 0;
	
	virtual void	InitNewMedia		(void) {};
};	


class CDROM_Interface_Image:public CDROM_Interface {
 unsigned unit;
public:
 static CDROM_Interface_Image *images[8];
 explicit CDROM_Interface_Image(unsigned n):unit(n){images[n]=this;}
 ~CDROM_Interface_Image(){if(images[unit]==this)images[unit]=nullptr;}
 bool SetDevice(char*,int) override;
 bool GetUPC(unsigned char&,char*)override{return false;}
 bool GetAudioTracks(int&,int&,TMSF&)override;
 bool GetAudioTrackInfo(int,TMSF&,unsigned char&)override;
 bool GetAudioSub(unsigned char&,unsigned char&,unsigned char&,TMSF&,TMSF&)override{return false;}
 bool GetAudioStatus(bool&playing,bool&pause)override{playing=pause=false;return true;}
 bool GetMediaTrayStatus(bool&,bool&,bool&)override;
 bool PlayAudioSector(unsigned long,unsigned long)override{return false;}
 bool PauseAudio(bool)override{return false;}
 bool StopAudio()override{return true;}
 void ChannelControl(TCtrl)override{}
 bool ReadSectors(PhysPt,bool,unsigned long,unsigned long)override;
 bool LoadUnloadMedia(bool)override{return false;}
 bool ReadSector(Bit8u*,bool,Bit32u);
 bool HasDataTrack(){return true;}
};

#define MSF_TO_FRAMES(M,S,F) ((M)*60*75+(S)*75+(F))
