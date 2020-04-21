#include "ftd_define.h"

int32 prdAvatar2ftdAvatar(const prdAvatar* pPrd, ftdAvatar* pFtd)
{
    int32 iRet = 0;
    pFtd->m_iAvatarId = pPrd->m_iAvatarId;
    iRet = WChar2Char(pPrd->m_strAvatarName, pFtd->m_strAvatarName, AVATARNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    return 0;
}

extern int32 ftdAvatar2prdAvatar(const ftdAvatar* pFtd, prdAvatar* pPrd)
{
    int32 iRet = 0;
    pPrd->m_iAvatarId = pFtd->m_iAvatarId;
    iRet = Char2WChar(pFtd->m_strAvatarName, pPrd->m_strAvatarName, AVATARNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    return 0;
}

int32 prdAvatarCreateData2ftdAvatarCreateData(const prdAvatarCreateData* pPrd, ftdAvatarCreateData* pFtd)
{
    int32 iRet = 0;
    iRet = WChar2Char(pPrd->m_strAvatarName, pFtd->m_strAvatarName, AVATARNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    return 0;
}

extern int32 ftdAvatarCreateData2prdAvatarCreateData(const ftdAvatarCreateData* pFtd, prdAvatarCreateData* pPrd)
{
    int32 iRet = 0;
    iRet = Char2WChar(pFtd->m_strAvatarName, pPrd->m_strAvatarName, AVATARNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    return 0;
}

int32 prdAvatarSelectData2ftdAvatarSelectData(const prdAvatarSelectData* pPrd, ftdAvatarSelectData* pFtd)
{
    int32 iRet = 0;
    iRet = WChar2Char(pPrd->m_strAvatarName, pFtd->m_strAvatarName, AVATARNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    pFtd->m_iAvatarId = pPrd->m_iAvatarId;
    pFtd->m_iLastChannelId = pPrd->m_iLastChannelId;
    return 0;
}

extern int32 ftdAvatarSelectData2prdAvatarSelectData(const ftdAvatarSelectData* pFtd, prdAvatarSelectData* pPrd)
{
    int32 iRet = 0;
    iRet = Char2WChar(pFtd->m_strAvatarName, pPrd->m_strAvatarName, AVATARNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    pPrd->m_iAvatarId = pFtd->m_iAvatarId;
    pPrd->m_iLastChannelId = pFtd->m_iLastChannelId;
    return 0;
}

int32 prdChannelData2ftdChannelData(const prdChannelData* pPrd, ftdChannelData* pFtd)
{
    int32 iRet = 0;
    iRet = WChar2Char(pPrd->m_strChannelName, pFtd->m_strChannelName, CHANNELNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    pFtd->m_iState = pPrd->m_iState;
    return 0;
}

extern int32 ftdChannelData2prdChannelData(const ftdChannelData* pFtd, prdChannelData* pPrd)
{
    int32 iRet = 0;
    iRet = Char2WChar(pFtd->m_strChannelName, pPrd->m_strChannelName, CHANNELNAME_MAX+1);
    if (iRet == 0)
    {
        return -1;
    }
    pPrd->m_iState = pFtd->m_iState;
    return 0;
}

