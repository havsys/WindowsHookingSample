//Author: Sky19938470
//2019/05/26



#include <windows.h>

#define HOOKDLL_EXPORTS
#include "HookDll.h"


// Shared data among all instances.
#pragma data_seg(".HOOKDATA")
HWND g_hWnd = NULL;	        // Window handle
HHOOK g_hHook = NULL;		// Hook handle

// Get module from address
HMODULE WINAPI ModuleFromAddress(PVOID pv) 
{
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
	{
		return (HMODULE)mbi.AllocationBase;
	}
	else
	{
		return NULL;
	}
}

// Hook callback
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	
    if (lParam & 0x40000000)	// Check the previous key state
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	
	// Post private messages to Main window
	// wParam specifies the virtual key code
	// lParam specifies the key data
    ::PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
	
    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Install or uninstall the hook
BOOL WINAPI SetKeyboardHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
	BOOL bOk;
	g_hWnd = hWndCaller;
	
	if (bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_KEYBOARD, KeyboardHookProc, 
			ModuleFromAddress(KeyboardHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else 
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	
	return bOk;
}


// Hook callback
LRESULT CALLBACK LowKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	
    if (lParam & 0x40000000)	// Check the previous key state
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}

	KBDLLHOOKSTRUCT  *pkbhs = (KBDLLHOOKSTRUCT *)lParam;

	//check that the message is from keyboard or is synthesized by SendInput API
	if((pkbhs->flags & LLKHF_INJECTED))
        return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if(wParam == WM_KEYDOWN)
	{
		// Post private messages to Main window
		// wParam specifies the virtual key code
		// lParam specifies the scan code
		::PostMessage(g_hWnd, WM_KEYINPUT, pkbhs->vkCode, pkbhs->scanCode);
	}
	
    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

BOOL WINAPI SetLowKeyboardHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
	BOOL bOk;
	g_hWnd = hWndCaller;
	
	if (bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, LowKeyboardHookProc, 
			ModuleFromAddress(LowKeyboardHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else 
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	
	return bOk;
}

//mouse hook proc
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);


	// Post private messages to Main window
	// wParam The identifier of the mouse message.
	// lParam A pointer to a MOUSEHOOKSTRUCT structure.
	::PostMessage(g_hWnd, WM_MOUSEEVENT, wParam, lParam);

	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Install or uninstall the hook
BOOL WINAPI SetMouseHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
	BOOL bOk;
	g_hWnd = hWndCaller;

	if (bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_MOUSE, MouseHookProc,
			ModuleFromAddress(MouseHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}

	return bOk;
}