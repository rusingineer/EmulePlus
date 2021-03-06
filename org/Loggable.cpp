#include "stdafx.h"
#ifndef NEW_SOCKETS_ENGINE
	#include "emule.h"
#else //NEW_SOCKETS_ENGINE
	#define GetResString(id)  _T("NOT IMPLEMENTED YET")
#endif //NEW_SOCKETS_ENGINE
#include "Loggable.h"

void CLoggable::AddLogLine(bool addtostatusbar, UINT nID, ...)
{
	va_list argptr;
	va_start(argptr, nID);
	AddLogText(false, addtostatusbar, GetResString(nID), argptr);
	va_end(argptr);
}

void CLoggable::AddLogLine(bool addtostatusbar, LPCTSTR line, ...)
{
	ASSERT(line != NULL);

	va_list argptr;
	va_start(argptr, line);
	AddLogText(false, addtostatusbar, line, argptr);
	va_end(argptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CLoggable::AddDebugLogLine(UINT nID, ...)
{
	va_list argptr;
	va_start(argptr, nID);
	AddLogText(true, false, GetResString(nID), argptr);
	va_end(argptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CLoggable::AddDebugLogLine(LPCTSTR line, ...)
{
	ASSERT(line != NULL);

	va_list argptr;
	va_start(argptr, line);
	AddLogText(true, false, line, argptr);
	va_end(argptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CLoggable::AddLogText(bool debug, bool addtostatusbar, LPCTSTR line, va_list argptr)
{
	ASSERT(line != NULL);

	EMULE_TRY

#ifndef NEW_SOCKETS_ENGINE
	if (debug && !g_App.m_pPrefs->GetVerbose())
		return;
#endif //NEW_SOCKETS_ENGINE

	CString sBuffer;

	sBuffer.FormatV(line, argptr);

#ifndef NEW_SOCKETS_ENGINE
	#ifdef _DEBUG
		//SyruS (0.29c) view the log messages while shutting down at least in the debugger
		if (g_App.m_app_state != g_App.APP_STATE_RUNNING)
			TRACE("App Log: %s\n", sBuffer);

	#endif
	if (g_App.m_pMDlg)
		g_App.m_pMDlg->AddLogText(addtostatusbar, sBuffer, debug);	// debug log and normal log handled by the same subroutine now
#else //NEW_SOCKETS_ENGINE
	g_stEngine.AddLog(debug ? LOG_DEBUG : LOG_WARNING, sBuffer);
#endif //NEW_SOCKETS_ENGINE

#ifdef _DEBUG
	if (debug)
		::OutputDebugString(sBuffer + _T("\n"));
#endif

	EMULE_CATCH2
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
