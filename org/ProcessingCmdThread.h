#pragma once

class CPartFile;
class CSharedFileList;

enum ProcCommand {PC_QUIT=0,PC_HASH};

struct Process_Cmd_struct // stucture to pass command
{
	long lSize; // size of the structure
	ProcCommand eCommand; // command passed
	CSharedFileList* pOwner;
	CPartFile* in_partfile_Owner;
	TCHAR *cFileName;
	TCHAR *cFolder;
};

struct PThread_Init_struct
{
	HANDLE hCommandPipe;
};

// CProcessingCmdThread

class CProcessingCmdThread : public CWinThread
{
	DECLARE_DYNCREATE(CProcessingCmdThread)

protected:
	CProcessingCmdThread();           // protected constructor used by dynamic creation
	virtual ~CProcessingCmdThread();
	void SetStopThread(bool bStop);
	bool IsThreadAboutToStop(void);

	HANDLE m_hCommandPipe;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	CMutex m_StopLock;
	bool m_bStopThread;
	DECLARE_MESSAGE_MAP()
public:
	CEvent m_QuitPCmdEvent;
	BOOL SetStartupData(PThread_Init_struct* pInitStruct);
	virtual int Run();
	static bool SendQuitCommand(HANDLE hControlPipe);
	void SetThreadToStopProcessing(void);
	static bool AddFileToHash(HANDLE hControlPipe,CSharedFileList* pOwner, const CString& strFolder, const CString& strFileName, CPartFile* pPartFileOwner);
protected:
	// Performs actual hashing of the file
	void HashFile(CSharedFileList* pOwner, const CString& strFolder, const CString& strFileName, CPartFile* pPartFileOwner);
	void ReleaseHashStruct(Process_Cmd_struct* pStruct);
public:

};


