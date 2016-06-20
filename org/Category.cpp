#include "stdafx.h"

#include "Category.h"
#ifndef NEW_SOCKETS_ENGINE
#include "emule.h"
#endif //NEW_SOCKETS_ENGINE
#include "ed2k_filetype.h"

CCat::CCatArray		CCat::g_arrCat;
CCat::CCatIDMap		CCat::g_mapCatID;	// Reverse lookup map for category IDs
EnumCategories		CCat::g_eAllCatType = CAT_ALL;
byte				CCat::g_iNumPredefinedCats = 0;

#ifndef NEW_SOCKETS_ENGINE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCat::CCat()
	: m_iPriority(0), m_bIsPredefined(false), m_eCatID(CAT_NONE), m_crColor(RGB(0, 0, 0))
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCat::CCat(EnumCategories ePredefinedCatID)
	: m_iPriority(0), m_bIsPredefined(false), m_eCatID(CAT_NONE)
{
	if (ePredefinedCatID >= CAT_PREDEFINED)
	{
		m_strTitle = GetPredefinedCatTitle(ePredefinedCatID,g_App.m_pPrefs->GetLanguageID());
		m_crColor = RGB(0, 0, 0);
		m_bIsPredefined = true;
		m_eCatID = ePredefinedCatID;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCat::CCat(LPCTSTR strTitle, LPCTSTR strSavePath/*=NULL*/, LPCTSTR strTempPath/*=NULL*/, LPCTSTR strComment/*=NULL*/,
		   LPCTSTR strAutoCatExt/*=NULL*/)
	: m_iPriority(0), m_bIsPredefined(false), m_eCatID(CAT_NONE)
{
	m_crColor = RGB(0, 0, 0);
	m_strTitle = strTitle;
	m_strSavePath = strSavePath;
	m_strTempPath = strTempPath;
	m_strComment = strComment;
	m_strAutoCatExt = strAutoCatExt;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void CCat::Finalize()
{
	CCat	*pCatToDel = NULL;

	while (!g_arrCat.IsEmpty())
	{
		pCatToDel = g_arrCat[0]; 
		g_arrCat.RemoveAt(0); 
		delete pCatToDel;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	GetNewCatID() returns the next available user category ID
/*static*/ EnumCategories CCat::GetNewCatID()
{
	byte			abyteUsedIdx[256];
	EnumCategories	eNewCatID = CAT_NONE;

	memzero(abyteUsedIdx, sizeof(abyteUsedIdx));
	for (int i = g_iNumPredefinedCats; i < g_arrCat.GetCount(); i++)
		abyteUsedIdx[g_arrCat[i]->m_eCatID] = 1;

//	Find minimal unused category index for new allocation
	for (unsigned ui = 1; ui < sizeof(abyteUsedIdx); ui++)
	{
		if (abyteUsedIdx[ui] == 0)
		{
			eNewCatID = static_cast<_EnumCategories>(ui);
			break;
		}
	}

	return eNewCatID;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MoveCat() moves the category at 'iFromIndex' to just before the category at 'iToIndex'.
//		It returns true on success and false on failure.
/*static*/ bool CCat::MoveCat(byte iFromIndex, byte iToIndex)
{
//	If either of the indices are past the end of the array
//		 or are the same, just return.
	if ( iFromIndex >= (UINT)g_arrCat.GetCount()
	  || iToIndex >= (UINT)g_arrCat.GetCount()+1
	  || iFromIndex == iToIndex )
	{
		return false;
	}

	CCat			*pCatToMove = NULL;

	pCatToMove = g_arrCat[iFromIndex];

//	If the source is to the left of the destination...
	if (iFromIndex < iToIndex)
	{
		g_arrCat.RemoveAt(iFromIndex);
		g_arrCat.InsertAt(iToIndex-1,pCatToMove);

	//	The indices of all cats from 'iFromIndex' to the end of the array have changed;
	//		Update the reverse lookup map.
		for (byte i = iFromIndex; i < g_arrCat.GetCount(); i++)
		{
			g_mapCatID[g_arrCat[i]->m_eCatID] = i;
		}
	}
//	If the source is to the right of the destination...
	else
	{
		g_arrCat.InsertAt(iToIndex,pCatToMove);
		g_arrCat.RemoveAt(iFromIndex+1);

	//	The indices of all cats from 'iToIndex' to the end of the array have changed;
	//		Update the reverse lookup map.
		for (byte i = iToIndex; i < g_arrCat.GetCount(); i++)
		{
			g_mapCatID[g_arrCat[i]->m_eCatID] = i;
		}
	}

	g_App.m_pPrefs->SaveCats();

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	AddCat() adds the new category 'pCat' and returns it's newly assigned index.
/*static*/ int CCat::AddCat(CCat *pCat, bool bAssignNewID/*=true*/)
{
	EnumCategories	eNewCatID;
	int				iIdx = g_arrCat.Add(pCat);

	if (bAssignNewID)
	{
		eNewCatID = GetNewCatID();
		pCat->m_eCatID = eNewCatID;
	}
	else
		eNewCatID = pCat->m_eCatID;
	g_mapCatID[eNewCatID] = static_cast<byte>(iIdx);

	return iIdx;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	AddPredefinedCat() adds the new predefined category 'pCat'.
/*static*/ int CCat::AddPredefinedCat(CCat *pCat)
{
	byte				iIndex = 0;

//	Find the index at which to insert the cat.
	for (iIndex = 0; iIndex <= GetNumPredefinedCats(); iIndex++)
	{
		if ( iIndex == GetNumPredefinedCats()
		  || g_arrCat[iIndex]->m_eCatID > pCat->m_eCatID )
		{
			break;
		}
	}

	g_arrCat.InsertAt(iIndex,pCat);
	g_iNumPredefinedCats++;

//	The indices of all cats from 'iIndex' to the end of the array have changed;
//		Update the reverse lookup map.
	for (byte i = iIndex; i < g_arrCat.GetCount(); i++)
	{
		g_mapCatID[g_arrCat[i]->m_eCatID] = i;
	}

	return iIndex;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	RemoveCat() removes and destroys the category at index 'iIndex'.
/*static*/ void CCat::RemoveCatByIndex(int iIndex)
{
//	If the index is within bounds...
	if (iIndex >= 0 && iIndex < g_arrCat.GetCount())
	{ 
		CCat	*pCatToDelete;

		pCatToDelete = g_arrCat[iIndex];

	//	If the category we're removing is predefined, reduce the predefined cat count.
		if (pCatToDelete->m_eCatID >= CAT_PREDEFINED)
			g_iNumPredefinedCats--;

		g_arrCat.RemoveAt(iIndex);

	//	The indices of all cats from 'iIndex' to the end of the array have changed;
	//		Update the reverse lookup map.
		g_mapCatID.erase(pCatToDelete->m_eCatID);
		for (byte i = iIndex; i < g_arrCat.GetCount(); i++)
		{
			g_mapCatID[g_arrCat[i]->m_eCatID] = i;
		}

		delete pCatToDelete;
	}
}
#endif //NEW_SOCKETS_ENGINE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ int CCat::GetCatIndexByID(const EnumCategories &eCatID)
{
	CCatIDMap::iterator		it;

	if ((it = g_mapCatID.find(eCatID)) != g_mapCatID.end())
		return it->second;
	else
		return -1;
}
#ifndef NEW_SOCKETS_ENGINE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool CCat::FileBelongsToGivenCat(CPartFile *file,EnumCategories eCatID, bool bIgnoreViewFilter)
{
//	Easy normal cases
	if (eCatID < CAT_PREDEFINED)
	{
		if (eCatID == file->GetCatID())
			return true;
		else
			return false;
	}

	if (eCatID == CAT_ALL && !bIgnoreViewFilter)
		eCatID = CCat::GetAllCatType();

	switch (eCatID)
	{
		case CAT_NONE:			return (eCatID == CAT_ALL);	// Hrm. What was I thinking here?
		case CAT_ALL:			return true;
		case CAT_UNCATEGORIZED:	return (file->GetCatID()==0);
		case CAT_INCOMPLETE:	return (file->IsPartFile());
		case CAT_COMPLETED:		return (!file->IsPartFile());
		case CAT_WAITING:		return ( (file->GetStatus()==PS_READY || file->GetStatus()==PS_EMPTY)
									  && file->GetTransferringSrcCount() == 0 );
		case CAT_DOWNLOADING:	return ( (file->GetStatus()==PS_READY || file->GetStatus()==PS_EMPTY)
									  && file->GetTransferringSrcCount() > 0 );
		case CAT_ERRONEOUS:		return (file->GetStatus()==PS_ERROR);
		case CAT_PAUSED:		return (file->GetStatus()==PS_PAUSED);
		case CAT_STOPPED:		return file->IsStopped();
		case CAT_STALLED:		return ( (file->GetStatus()==PS_READY || file->GetStatus()==PS_EMPTY)
									  && file->GetTransferringSrcCount() == 0
									  && file->IsStalled() );
		case CAT_ACTIVE:		return ( (file->GetStatus()==PS_READY || file->GetStatus()==PS_EMPTY)
									  && (file->GetTransferringSrcCount() > 0 || file->GetOnQueueSrcCount() != 0)
									  && file->IsPartFile() );
		case CAT_INACTIVE:		return !( (file->GetStatus()==PS_READY || file->GetStatus()==PS_EMPTY)
									  && (file->GetTransferringSrcCount() > 0 || file->GetOnQueueSrcCount() != 0) )
									  && file->IsPartFile();
		case CAT_VIDEO:			return file->IsMovie();
		case CAT_AUDIO:			return (ED2KFT_AUDIO == file->GetFileType());
		case CAT_ARCHIVES:		return file->IsArchive();
		case CAT_CDIMAGES:		return (ED2KFT_CDIMAGE == file->GetFileType());
	}

	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ CString CCat::GetPredefinedCatTitle(EnumCategories eCatID, int langID)
{
	UINT		dwResStrId;

	switch (eCatID)
	{
		case CAT_ALL:
			dwResStrId = IDS_CAT_ALL;
			break;
		case CAT_UNCATEGORIZED:
			dwResStrId = IDS_CAT_UNCATEGORIZED;
			break;
		case CAT_INCOMPLETE:
			dwResStrId = IDS_CAT_INCOMPLETE;
			break;
		case CAT_COMPLETED:
			dwResStrId = IDS_COMPLETE;
			break;
		case CAT_WAITING:
			dwResStrId = IDS_WAITING;
			break;
		case CAT_DOWNLOADING:
			dwResStrId = IDS_DOWNLOADING;
			break;
		case CAT_ERRONEOUS:
			dwResStrId = IDS_ERRORLIKE;
			break;
		case CAT_PAUSED:
			dwResStrId = IDS_PAUSED;
			break;
		case CAT_STOPPED:
			dwResStrId = IDS_STOPPED;
			break;
		case CAT_STALLED:
			dwResStrId = IDS_STALLED;
			break;
		case CAT_ACTIVE:
			dwResStrId = IDS_ST_ACTIVE;
			break;
		case CAT_INACTIVE:
			dwResStrId = IDS_ST_INACTIVE;
			break;
		case CAT_VIDEO:
			dwResStrId = IDS_VIDEO;
			break;
		case CAT_AUDIO:
			dwResStrId = IDS_AUDIO;
			break;
		case CAT_ARCHIVES:
			dwResStrId = IDS_SEARCH_ARC;
			break;
		case CAT_CDIMAGES:
			dwResStrId = IDS_SEARCH_CDIMG;
			break;
		default:
			return CString(_T("?"));
	}
	return GetResString(dwResStrId, (langID == 0) ? g_App.m_pPrefs->GetLanguageID() : langID);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //NEW_SOCKETS_ENGINE
