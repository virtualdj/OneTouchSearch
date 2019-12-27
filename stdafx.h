// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include <windows.h>
#define _AFXDLL
#include <afxwin.h>
#include <afxtempl.h>

#include <shellapi.h>
#include <map>
#include <string>
#include <assert.h>

#include "OneTouchSearchAction.h"
#include "TrayIcon.h"

#ifndef HINST_THISCOMPONENT
    EXTERN_C IMAGE_DOS_HEADER __ImageBase;
    #define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// TODO: reference additional headers your program requires here
#endif