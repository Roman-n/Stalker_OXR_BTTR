#include "stdafx.h"
#include "player_hud.h"
#include "Level.h"
#include "debug_renderer.h"
#include "xrEngine/xr_input.h"
#include "HudManager.h"
#include "HudItem.h"
#include "xrEngine/Effector.h"
#include "xrEngine/CameraManager.h"
#include "xrEngine/FDemoRecord.h"
#include "ui_base.h"
#include "debug_renderer.h"
#include "xrEngine/GameFont.h"

u32 hud_adj_mode = 0;
u32 hud_adj_item_idx = 0;

float _delta_pos = 0.0005f;
float _delta_rot = 0.05f;

bool is_attachable_item_tuning_mode()
{
    return pInput->iGetAsyncKeyState(DIK_LSHIFT) || pInput->iGetAsyncKeyState(DIK_Z) ||
        pInput->iGetAsyncKeyState(DIK_X) || pInput->iGetAsyncKeyState(DIK_C);
}

void tune_remap(const Ivector& in_values, Ivector& out_values)
{
    if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
    {
        out_values = in_values;
    }
    else if (pInput->iGetAsyncKeyState(DIK_Z))
    { // strict by X
        out_values.x = in_values.y;
        out_values.y = 0;
        out_values.z = 0;
    }
    else if (pInput->iGetAsyncKeyState(DIK_X))
    { // strict by Y
        out_values.x = 0;
        out_values.y = in_values.y;
        out_values.z = 0;
    }
    else if (pInput->iGetAsyncKeyState(DIK_C))
    { // strict by Z
        out_values.x = 0;
        out_values.y = 0;
        out_values.z = in_values.y;
    }
    else
    {
        out_values.set(0, 0, 0);
    }
}

void calc_cam_diff_pos(Fmatrix item_transform, Fvector diff, Fvector& res)
{
    Fmatrix cam_m;
    cam_m.i.set(Device.vCameraRight);
    cam_m.j.set(Device.vCameraTop);
    cam_m.k.set(Device.vCameraDirection);
    cam_m.c.set(Device.vCameraPosition);

    Fvector res1;
    cam_m.transform_dir(res1, diff);

    Fmatrix item_transform_i;
    item_transform_i.invert(item_transform);
    item_transform_i.transform_dir(res, res1);
}

void calc_cam_diff_rot(Fmatrix item_transform, Fvector diff, Fvector& res)
{
    Fmatrix cam_m;
    cam_m.i.set(Device.vCameraRight);
    cam_m.j.set(Device.vCameraTop);
    cam_m.k.set(Device.vCameraDirection);
    cam_m.c.set(Device.vCameraPosition);

    Fmatrix R;
    R.identity();
    if (!fis_zero(diff.x))
    {
        R.rotation(cam_m.i, diff.x);
    }
    else if (!fis_zero(diff.y))
    {
        R.rotation(cam_m.j, diff.y);
    }
    else if (!fis_zero(diff.z))
    {
        R.rotation(cam_m.k, diff.z);
    };

    Fmatrix item_transform_i;
    item_transform_i.invert(item_transform);
    R.mulB_43(item_transform);
    R.mulA_43(item_transform_i);

    R.getHPB(res);

    res.mul(180.0f / PI);
}

void hud_draw_adjust_mode()
{
    if (!hud_adj_mode)
        return;

    LPCSTR _text = NULL;
    if (pInput->iGetAsyncKeyState(DIK_LSHIFT) && hud_adj_mode)
        _text =
            "press SHIFT+NUM 0-return 1-hud_pos 2-hud_rot 3-itm_pos 4-itm_rot 5-fire_point 6-fire_2_point "
            "7-shell_point "
            "8-pos_step 9-rot_step";

    switch (hud_adj_mode)
    {
    case 1: _text = "adjusting HUD POSITION"; break;
    case 2: _text = "adjusting HUD ROTATION"; break;
    case 3: _text = "adjusting ITEM POSITION"; break;
    case 4: _text = "adjusting ITEM ROTATION"; break;
    case 5: _text = "adjusting FIRE POINT"; break;
    case 6: _text = "adjusting FIRE 2 POINT"; break;
    case 7: _text = "adjusting SHELL POINT"; break;
    case 8: _text = "adjusting pos STEP"; break;
    case 9: _text = "adjusting rot STEP"; break;
    };
    if (_text)
    {
        CGameFont* F = UI().Font().pFontDI;
        F->SetAligment(CGameFont::alCenter);
        F->OutSetI(0.f, -0.8f);
        F->SetColor(0xffffffff);
        F->OutNext(_text);
        F->OutNext("for item [%d]", hud_adj_item_idx);
        F->OutNext("delta values dP=%f dR=%f", _delta_pos, _delta_rot);
        F->OutNext("[Z]-x axis [X]-y axis [C]-z axis");
    }
}

void hud_adjust_mode_keyb(int dik)
{
    if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
    {
        if (dik == DIK_NUMPAD0)
            hud_adj_mode = 0;
        if (dik == DIK_NUMPAD1)
            hud_adj_mode = 1;
        if (dik == DIK_NUMPAD2)
            hud_adj_mode = 2;
        if (dik == DIK_NUMPAD3)
            hud_adj_mode = 3;
        if (dik == DIK_NUMPAD4)
            hud_adj_mode = 4;
        if (dik == DIK_NUMPAD5)
            hud_adj_mode = 5;
        if (dik == DIK_NUMPAD6)
            hud_adj_mode = 6;
        if (dik == DIK_NUMPAD7)
            hud_adj_mode = 7;
        if (dik == DIK_NUMPAD8)
            hud_adj_mode = 8;
        if (dik == DIK_NUMPAD9)
            hud_adj_mode = 9;
    }
    if (pInput->iGetAsyncKeyState(DIK_LCONTROL))
    {
        if (dik == DIK_NUMPAD0)
            hud_adj_item_idx = 0;
        if (dik == DIK_NUMPAD1)
            hud_adj_item_idx = 1;
    }
}
