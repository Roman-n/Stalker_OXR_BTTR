// xrRender_GL.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "Layers/xrRender/dxRenderFactory.h"
#include "Layers/xrRender/dxUIRender.h"
#include "Layers/xrRender/dxDebugRender.h"

extern "C"
{
XR_EXPORT void SetupEnv()
{
    GEnv.Render = &RImplementation;
    GEnv.RenderFactory = &RenderFactoryImpl;
    GEnv.DU = &DUImpl;
    GEnv.UIRender = &UIRenderImpl;
    xrRender_initconsole();
}

XR_EXPORT bool CheckRendererSupport()
{
    // XXX: do a real check
    return true;
}

}
