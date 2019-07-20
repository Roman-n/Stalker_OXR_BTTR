#include "stdafx.h"
#ifdef __DISCORD_API
#include "Window.h"

XRay::Window::Window(IntPtr InNativeObject)
{
	CAST_TO_NATIVE_OBJECT(CUIWindow, InNativeObject);
}
#endif