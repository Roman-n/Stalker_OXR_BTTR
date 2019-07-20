#include "stdafx.h"
#include "StringsUtils.h"
#include <process.h>
#include <VersionHelpers.h>

using ThreadCall = HRESULT(WINAPI *)(HANDLE handle, PCWSTR name);

static bool Win10SupportNewThreadNameInit = false;
static ThreadCall pThreadCall = nullptr;

struct	THREAD_STARTUP
{
	thread_t* entry;
	char* name;
	void* args;
};

#pragma pack(push,8)
struct THREAD_NAME
{
	DWORD dwType;
	const char* szName;
	DWORD dwThreadID;
	DWORD dwFlags;
};

