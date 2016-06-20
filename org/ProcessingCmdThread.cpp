// ProcessingCmdThread.cpp : implementation file
//

#include "stdafx.h"
#ifndef NEW_SOCKETS_ENGINE
	#include "eMule.h"
#else
	#include "otherfunctions.h"
	#include "Engine/Files/TaskProcessorFiles.h"
#endif //NEW_SOCKETS_ENGINE
#include "ProcessingCmdThread.h"
#include "KnownFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNCREATE(CProcessingCmdThread, CWinThread)

CProcessingCmdThread::CProcessingCmdThread()
{
	m_hCommandPipe	=	NULL;
	SetStopThread(false);
}

CProcessingCmdThread::~CProcessingCmdThread()
{
	if ( m_hCommandPipe )
	{
		CloseHandle(m_hCommandPipe);
		m_hCommandPipe	=	NULL;
	}
}

BOOL CProcessingCmdThread::InitInstance()
{
	if ( !m_hCommandPipe || m_bStopThread )
	{
		return FALSE;
	}
	return TRUE;
}

int CProcessingCmdThread::ExitInstance()
{
//	TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProcessingCmdThread, CWinThread)
END_MESSAGE_MAP()


// CProcessingCmdThread message handlers

BOOL CProcessingCmdThread::SetStartupData(PThread_Init_struct* pInitStruct)
{
	if ( !pInitStruct )
	{
		return FALSE;
	}
	if ( pInitStruct->hCommandPipe == NULL )
	{
		return FALSE;
	}
	m_hCommandPipe	=	pInitStruct->hCommandPipe;
	delete[] pInitStruct;
	return TRUE;
}

int CProcessingCmdThread::Run()
{
	bool bReadPipeOK;
	DWORD dwBytesActuallyRead;
	bool bBustedPrioraty	=	false;
	bool bQuitCycle	=	false;
	if ( !m_hCommandPipe  )
	{
		return -1;
	}

	do
	{
		Process_Cmd_struct Struct;
		// Read From pipe , in case of now data it just waits on pipe 
		bReadPipeOK = ::ReadFile(m_hCommandPipe,(LPVOID)&Struct,(DWORD)sizeof(Process_Cmd_struct),&dwBytesActuallyRead,NULL);	
		// if pipe read OK 
		if(bReadPipeOK &&(dwBytesActuallyRead == sizeof(Process_Cmd_struct)) )
		{
			// if we started and found out that we should to exit soon
			// we shouldn't process any messages , just release the memory in the queue
			// until we get quit message , we need this to deallocate all string that was allocated in passed
			// Struct.cFileName and Struct.cFolder to avoid memory leaks
			if ( IsThreadAboutToStop()&& Struct.eCommand != PC_QUIT )
			{
				// we need to bust the prioraty to exit fast
				// and it's OK , since no processing is done here
				if ( !bBustedPrioraty )
				{
#ifndef NEW_SOCKETS_ENGINE
					SetThreadPriority(THREAD_PRIORITY_NORMAL+ g_App.m_pPrefs->GetMainProcessPriority());
#endif //NEW_SOCKETS_ENGINE
					// do it only once
					bBustedPrioraty	=	true;
				}
				// clear the memory used
				ReleaseHashStruct(&Struct);
				// and exit while (on next iteration)
				continue;
			}

			switch (Struct.eCommand)
			{
			// command to create file hash
			case PC_HASH:
				HashFile(Struct.pOwner,Struct.cFolder,Struct.cFileName,Struct.in_partfile_Owner);
				break;
			// command for thread to exit
			case PC_QUIT:
				// it's actually double check but just to be sure
				SetStopThread(true);
				// break the loop 
				bQuitCycle	=	true;
				break;
			default:
				TRACE(_T("Invalid command received"));
			};
		}
		else
		{
			// something wrong , we defenatly need to report it somehow
			TRACE(_T("Error reading from pipe or invalid data received"));
			Sleep(20); // let it sleep on it , maybe it will fix the problem
		}
		// Clear the memory used by structure
		ReleaseHashStruct(&Struct);
		Sleep(0);
	}
	while(!bQuitCycle);
	// signal that we finished here
	m_QuitPCmdEvent.SetEvent();
	if ( m_hCommandPipe )
	{
		CloseHandle(m_hCommandPipe);
		m_hCommandPipe	=	NULL;
	}
	return 0;
}

bool CProcessingCmdThread::SendQuitCommand(HANDLE hControlPipe)
{
	bool bWriteOK			=	false;
	DWORD dwBytesWritten	=	0;

	if ( !hControlPipe )
	{
		return false;
	}

	Process_Cmd_struct QuitCommandStrct;
	memzero(&QuitCommandStrct,sizeof(Process_Cmd_struct));
	QuitCommandStrct.lSize	=	sizeof(Process_Cmd_struct);
	QuitCommandStrct.eCommand	=	PC_QUIT;
	bWriteOK = WriteFile(hControlPipe,          // handle to file
					&QuitCommandStrct,              // data buffer
					sizeof(Process_Cmd_struct),     // number of bytes to write
					&dwBytesWritten,  // number of bytes written
					NULL        // overlapped buffer
					);
	if(!bWriteOK || (dwBytesWritten != sizeof(Process_Cmd_struct)))
	{
		bWriteOK = false;
	}

	return bWriteOK;
}

void CProcessingCmdThread::SetStopThread(bool bStop)
{
	CSingleLock (&m_StopLock,TRUE);
	m_bStopThread	=	bStop;
}

bool CProcessingCmdThread::IsThreadAboutToStop(void)
{
	CSingleLock (&m_StopLock,TRUE);
	return m_bStopThread;
}

// we want to allow only stop processing to be set from outside class
// thats why we use this public function
void CProcessingCmdThread::SetThreadToStopProcessing(void)
{
	SetStopThread(true);
}

// Performs actual hashing of the file
void CProcessingCmdThread::HashFile(CSharedFileList* pOwner, const CString& strFolder, const CString& strFileName, CPartFile* pPartFileOwner)
{
	// Create new Known file object
	CKnownFile* newrecord = new CKnownFile();
	
	if (newrecord)
	{
		// Run creation of the HASH data from file
		if (newrecord->CreateFromFile(strFolder, strFileName, (pOwner == NULL)))
		{
#ifndef NEW_SOCKETS_ENGINE
			// If main App window is exists and valid
			if (!IsThreadAboutToStop() && ::IsWindow(g_App.m_pMDlg->m_hWnd))
			{
				// post message ? - bit unsafe , it assumes that window getting this message will at some point
				// release the "newrecord" theoretically in some border cases it can cause small memory leak , but it's unlikly
				PostMessage(g_App.m_pMDlg->m_hWnd,TM_FINISHEDHASHING,(pOwner ? 0:(WPARAM)pPartFileOwner),(LPARAM)newrecord);
			}
			// if window is not valid
			else
			{
				delete newrecord;	
			}
#else
			CTask_FileHashed* pTask = new CTask_FileHashed(pOwner ? NULL : pPartFileOwner, newrecord);
			g_stEngine.Files.Push(pTask);
#endif //NEW_SOCKETS_ENGINE
		}
		else
		{
#ifndef NEW_SOCKETS_ENGINE
			//	Notify main program of hash failure
			if (pOwner && g_App.m_pMDlg && g_App.m_pMDlg->IsRunning())
			{
				UnknownFile_Struct	*pHashed = new UnknownFile_Struct;

				pHashed->m_strDirectory = strFolder;
				pHashed->m_strFileName = strFileName;
				PostMessage(g_App.m_pMDlg->m_hWnd, TM_HASHFAILED, 0, reinterpret_cast<LPARAM>(pHashed));
			}
#endif //NEW_SOCKETS_ENGINE
			delete newrecord;
		}
	}
}

// Adds file to hashing queue
bool CProcessingCmdThread::AddFileToHash(HANDLE hControlPipe,CSharedFileList* pOwner, const CString& strFolder, const CString& strFileName, CPartFile* pPartFileOwner)
{
	bool	bWriteOK = false;
	DWORD	dwBytesWritten = 0;

	// Check if all internal structures are valid
	if (!hControlPipe)
		return false;

	Process_Cmd_struct SendCommandStrct;
	memzero(&SendCommandStrct,sizeof(Process_Cmd_struct));
	// Set the transffer structure with processing data
	SendCommandStrct.lSize					=	sizeof(Process_Cmd_struct);
	SendCommandStrct.eCommand				=	PC_HASH;
	SendCommandStrct.pOwner					=	pOwner;
	SendCommandStrct.in_partfile_Owner		=	pPartFileOwner;
	SendCommandStrct.cFileName				=	new TCHAR[strFileName.GetLength() + 1];
	_tcscpy(SendCommandStrct.cFileName, strFileName);
	SendCommandStrct.cFolder				=	new TCHAR[strFolder.GetLength() + 1];
	_tcscpy(SendCommandStrct.cFolder, strFolder);
	// Write the structure to pipe
	bWriteOK = WriteFile(hControlPipe,          // handle to file
					&SendCommandStrct,              // data buffer
					sizeof(Process_Cmd_struct),     // number of bytes to write
					&dwBytesWritten,  // number of bytes written
					NULL        // overlapped buffer
					);
	// Check if write completed successfully
	if(!bWriteOK || (dwBytesWritten != sizeof(Process_Cmd_struct)))
	{
		bWriteOK = false;
	}

	return bWriteOK;
}
void CProcessingCmdThread::ReleaseHashStruct(Process_Cmd_struct* pStruct)
{
	if (pStruct->cFileName)
	{
		delete[] pStruct->cFileName;
		pStruct->cFileName	=	NULL;
	}
	if ( pStruct->cFolder )
	{
		delete[] pStruct->cFolder;
		pStruct->cFolder	=	NULL;
	}
}
