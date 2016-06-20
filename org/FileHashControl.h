#pragma once

class CProcessingCmdThread;
class CSharedFileList;
class CPartFile;

enum ProcessThreadPriority
{
	ePStandard	=	THREAD_PRIORITY_BELOW_NORMAL,
	ePLower		=	THREAD_PRIORITY_LOWEST ,
	ePIdle		=	THREAD_PRIORITY_IDLE ,
};

class CFileHashControl
{
protected:
	HANDLE m_hCommandPipe;
	CProcessingCmdThread* m_pProcessthread;
	ProcessThreadPriority m_eThreadPriority;
public:
	CFileHashControl(void);
	virtual ~CFileHashControl(void);
	HRESULT Init(void);
	HRESULT Destroy(void);
	// Adds file to the hash thread queue to be hashed
	HRESULT AddToHash(CSharedFileList*  pOwner, CString strInFolder, CString strFileName, CPartFile* in_partfile_Owner = NULL);
protected:
	bool m_bInitialized;
	HRESULT CreateProcessingThread(void);
public:
	HRESULT KillProcessCmdThread(void);
	void SetProcessingPriority(ProcessThreadPriority ePriority);
	ProcessThreadPriority GetProcessPriority(void);
};

