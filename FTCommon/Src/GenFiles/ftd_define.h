#ifndef _H_ftd_DEFINE
#define _H_ftd_DEFINE

#include "common.h"

struct ftdAvatar
{
    uint64 m_iAvatarId;
    char m_strAvatarName[AVATARNAME_MAX+1];
	ftdAvatar(){memset(this, 0, sizeof(ftdAvatar));}
};

struct prdAvatar
{
    uint64 m_iAvatarId;
    TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	prdAvatar(){memset(this, 0, sizeof(prdAvatar));}
};

extern int32 prdAvatar2ftdAvatar(const prdAvatar* pPrd, ftdAvatar* pFtd);
extern int32 ftdAvatar2prdAvatar(const ftdAvatar* pFtd, prdAvatar* pPrd);

struct ftdAvatarCreateData
{
    char m_strAvatarName[AVATARNAME_MAX+1];
	ftdAvatarCreateData(){memset(this, 0, sizeof(ftdAvatarCreateData));}
};

struct prdAvatarCreateData
{
    TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	prdAvatarCreateData(){memset(this, 0, sizeof(prdAvatarCreateData));}
};

extern int32 prdAvatarCreateData2ftdAvatarCreateData(const prdAvatarCreateData* pPrd, ftdAvatarCreateData* pFtd);
extern int32 ftdAvatarCreateData2prdAvatarCreateData(const ftdAvatarCreateData* pFtd, prdAvatarCreateData* pPrd);

struct ftdAvatarSelectData
{
    char m_strAvatarName[AVATARNAME_MAX+1];
    uint64 m_iAvatarId;
    uint8 m_iLastChannelId;
	ftdAvatarSelectData(){memset(this, 0, sizeof(ftdAvatarSelectData));}
};

struct prdAvatarSelectData
{
    TCHAR m_strAvatarName[AVATARNAME_MAX+1];
    uint64 m_iAvatarId;
    uint8 m_iLastChannelId;
	prdAvatarSelectData(){memset(this, 0, sizeof(prdAvatarSelectData));}
};

extern int32 prdAvatarSelectData2ftdAvatarSelectData(const prdAvatarSelectData* pPrd, ftdAvatarSelectData* pFtd);
extern int32 ftdAvatarSelectData2prdAvatarSelectData(const ftdAvatarSelectData* pFtd, prdAvatarSelectData* pPrd);

struct ftdChannelData
{
    char m_strChannelName[CHANNELNAME_MAX+1];
    uint8 m_iState;
	ftdChannelData(){memset(this, 0, sizeof(ftdChannelData));}
};

struct prdChannelData
{
    TCHAR m_strChannelName[CHANNELNAME_MAX+1];
    uint8 m_iState;
	prdChannelData(){memset(this, 0, sizeof(prdChannelData));}
};

extern int32 prdChannelData2ftdChannelData(const prdChannelData* pPrd, ftdChannelData* pFtd);
extern int32 ftdChannelData2prdChannelData(const ftdChannelData* pFtd, prdChannelData* pPrd);

#endif