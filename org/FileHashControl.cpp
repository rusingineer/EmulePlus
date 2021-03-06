#include "stdafx.h"
#include "filehashcontrol.h"
#include "ProcessingCmdThread.h"
#ifndef NEW_SOCKETS_ENGINE
	#include "emule.h"
#else
	#include "otherfunctions.h"
#endif //NEW_SOCKETS_ENGINE

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CFileHashControl::CFileHashControl(void)
: m_bInitialized(false)
{
	m_hCommandPipe		=	NULL;
	m_pProcessthread	=	NULL;
	m_eThreadPriority	=	ePStandard;
}

CFileHashControl::~CFileHashControl(void)
{
	if ( m_bInitialized )
	{
		Destroy();
	}
}

HRESULT CFileHashControl::Init(void)
{
	HRESULT hr;

	if ( m_bInitialized )
	{
		// already initialized
		return S_FALSE;
	}

	hr	=	CreateProcessingThread();
	if ( FAILED(hr) )
	{
		return hr;
	}
	m_bInitialized	=	true;
	return S_OK;
}

HRESULT CFileHashControl::Destroy(void)
{
	HRESULT hr	=	S_OK;
	if ( !m_bInitialized )
	{
		return S_OK;
	}
	if ( !m_hCommandPipe || !m_pProcessthread )
	{
		// something wrong with pointers but no need to release
		if ( m_hCommandPipe )
		{
			::CloseHandle(m_hCommandPipe);
		}
		return S_FALSE;
	}
	// try to kill the thread 
	hr	=	KillProcessCmdThread();
	// reset pointers
	m_bInitialized		=	false;
	m_pProcessthread	=	NULL;
	return hr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Adds file to the hash thread queue to be hashed
HRESULT CFileHashControl::AddToHash(CSharedFileList*  pOwner, CString strInFolder, CString strFileName, CPartFile* in_partfile_Owner)
{
	// Check if the current state is OK
	if ( !m_hCommandPipe || !m_bInitialized )
	{
		return E_UNEXPECTED;
	}
	// Try to add file to the queue
	if ( !m_pProcessthread->AddFileToHash(m_hCommandPipe,pOwner,strInFolder,strFileName,in_partfile_Owner) )
	{
		return E_FAIL;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CFileHashControl::CreateProcessingThread(void)
{
	HRESULT hr	=	S_OK;
	if ( m_pProcessthread || m_bInitialized )
	{
		return E_UNEXPECTED;
	}
	m_pProcessthread = (CProcessingCmdThread*) AfxBeginThread(RUNTIME_CLASS(CProcessingCmdThread), m_eThreadPriority,0, CREATE_SUSPENDED);
	if ( !m_pProcessthread )
	{
		return E_FAIL;
	}
	// Allocate initializtion structure 
	PThread_Init_struct *pIStruct = new PThread_Init_struct;
	if ( !pIStruct )
	{
		hr	=	E_OUTOFMEMORY;
	}
	else
	{
		// Reset it to 0
		memzero(pIStruct,sizeof(PThread_Init_struct));

		// Create the security attribute needed
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
		saAttr.bInheritHandle = TRUE; 
		saAttr.lpSecurityDescriptor = NULL; 

		// Create pipe for commands 
		BOOL bCreatePipe = ::CreatePipe(&(pIStruct->hCommandPipe),&m_hCommandPipe,
				&saAttr,1024*sizeof(Process_Cmd_struct*)// pipe saggested Size , needed relatively big
				);
		if ( !bCreatePipe ) // if failed to create
		{
			delete[] pIStruct;
			hr	=	E_FAIL;
		}
		else
		{
			// Set startup data to the thread
			if ( !m_pProcessthread->SetStartupData(pIStruct))
			{
				delete[] pIStruct;
				::CloseHandle(m_hCommandPipe);
				m_hCommandPipe	=	NULL;
				hr	=	E_FAIL;
			}
		}
	}
	// Resume thread (make it Run) , if the thread initialization failed it will suiside right away
	m_pProcessthread->ResumeThread();	
	// if we had error on the way - reinitialize the value 
	if ( FAILED(hr) )
	{
		m_pProcessthread	=	NULL;
	}
	// Finished , return the result
	return hr;
}

HRESULT CFileHashControl::KillProcessCmdThread(void)
{
	if ( ! m_pProcessthread || !m_bInitialized || !m_hCommandPipe )
	{
		// if thread not initialized of pointer is NULL return success but a bit different
		return S_FALSE;
	}
	// set stop flag
	m_pProcessthread->SetThreadToStopProcessing();
	// send stop command
	m_pProcessthread->SendQuitCommand(m_hCommandPipe);
	// wait until thread exits - Cax2 - it was returning E_UNEXPECTED when it should have returned S_OK & viceversa...
	bool exited=m_pProcessthread->m_QuitPCmdEvent.Lock(1000000);
	CloseHandle(m_hCommandPipe);
	m_hCommandPipe	=	NULL;
	return (exited)? S_OK:E_UNEXPECTED;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SetProcessingPriority() sets the runtime priority of the Hashing Thread.
void CFileHashControl::SetProcessingPriority(ProcessThreadPriority ePriority)
{
#ifndef NEW_SOCKETS_ENGINE
	m_eThreadPriority = ePriority;

	if (m_pProcessthread && m_bInitialized)
	{
		m_pProcessthread->SetThreadPriority(m_eThreadPriority + g_App.m_pPrefs->GetMainProcessPriority());
	}
#endif //NEW_SOCKETS_ENGINE
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProcessThreadPriority CFileHashControl::GetProcessPriority(void)
{
	return m_eThreadPriority;
}
