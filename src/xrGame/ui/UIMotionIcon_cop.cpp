#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon_cop.h"
#include "UIXmlInit.h"
const LPCSTR MOTION_ICON_XML = "motion_icon_cop.xml";

const LPCSTR MOTION_ICON_XML_NULL = "motion_icon_cop_null.xml";

extern int __type_hud_cop;

CUIMotionIcon_cop* g_pMotionIcon_cop = NULL;

CUIMotionIcon_cop::CUIMotionIcon_cop()
{
    g_pMotionIcon_cop = this;
    m_bchanged = true;
    m_luminosity = 0.0f;
    cur_pos = 0.f;
}

CUIMotionIcon_cop::~CUIMotionIcon_cop() { g_pMotionIcon_cop = NULL; }
void CUIMotionIcon_cop::ResetVisibility()
{
    m_npc_visibility.clear();
    m_bchanged = true;
}

void CUIMotionIcon_cop::Init(Frect const& zonemap_rect)
{
    CUIXml uiXml;
	if (__type_hud_cop)
	{
		uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML);
	}
	else
	{
		uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, MOTION_ICON_XML_NULL);
	}	
    CUIXmlInit xml_init;

    xml_init.InitWindow(uiXml, "window", 0, this);
    float rel_sz = uiXml.ReadAttribFlt("window", 0, "rel_size", 1.0f);
    Fvector2 sz;
    Fvector2 pos;
    zonemap_rect.getsize(sz);

    pos.set(sz.x / 2.0f, sz.y / 2.0f);
    SetWndSize(sz);
    SetWndPos(pos);

    float k = UI().get_current_kx();
    sz.mul(rel_sz * k);

    // float h = Device.dwHeight;
    // float w = Device.dwWidth;
    AttachChild(&m_luminosity_progress);
    xml_init.InitProgressShape(uiXml, "luminosity_progress", 0, &m_luminosity_progress);
    m_luminosity_progress.SetWndSize(sz);
    m_luminosity_progress.SetWndPos(pos);

    AttachChild(&m_noise_progress);
    xml_init.InitProgressShape(uiXml, "noise_progress", 0, &m_noise_progress);
    m_noise_progress.SetWndSize(sz);
    m_noise_progress.SetWndPos(pos);
}

void CUIMotionIcon_cop::SetNoise(float Pos)
{
    if (!IsGameTypeSingle())
        return;
    if (!IsShown())
        return;
    Pos = clampr(Pos, 0.f, 100.f);
    m_noise_progress.SetPos(Pos / 100.f);
}

void CUIMotionIcon_cop::SetLuminosity(float Pos)
{
    if (!IsGameTypeSingle())
        return;
    if (!IsShown())
        return;
    m_luminosity = Pos;
}

void CUIMotionIcon_cop::Draw()
{
    if (!IsShown())
        return;
    inherited::Draw();
}
void CUIMotionIcon_cop::Update()
{
    if (!IsGameTypeSingle())
    {
        inherited::Update();
        return;
    }
    if (!IsShown())
        return;
    if (m_bchanged)
    {
        m_bchanged = false;
        if (m_npc_visibility.size())
        {
            std::sort(m_npc_visibility.begin(), m_npc_visibility.end());
            SetLuminosity(m_npc_visibility.back().value);
        }
        else
            SetLuminosity(0.f);
    }
    inherited::Update();

    // m_luminosity_progress
    if (cur_pos != m_luminosity)
    {
        float _diff = _abs(m_luminosity - cur_pos);
        if (m_luminosity > cur_pos)
        {
            cur_pos += _diff * Device.fTimeDelta;
        }
        else
        {
            cur_pos -= _diff * Device.fTimeDelta;
        }
        clamp(cur_pos, 0.f, 100.f);
        m_luminosity_progress.SetPos(cur_pos / 100.f);
    }
}

void SetActorVisibility(u16 who_id, float value)
{
    if (!IsGameTypeSingle())
        return;

    if (g_pMotionIcon_cop && g_pMotionIcon_cop->IsShown())
        g_pMotionIcon_cop->SetActorVisibility(who_id, value);
}

void CUIMotionIcon_cop::SetActorVisibility(u16 who_id, float value)
{
    if (!IsShown())
        return;

    clamp(value, 0.f, 1.f);
    value *= 100.f;

    xr_vector<_npc_visibility>::iterator it = std::find(m_npc_visibility.begin(), m_npc_visibility.end(), who_id);

    if (it == m_npc_visibility.end() && value != 0)
    {
        m_npc_visibility.resize(m_npc_visibility.size() + 1);
        _npc_visibility& v = m_npc_visibility.back();
        v.id = who_id;
        v.value = value;
    }
    else if (fis_zero(value))
    {
        if (it != m_npc_visibility.end())
            m_npc_visibility.erase(it);
    }
    else
    {
        (*it).value = value;
    }

    m_bchanged = true;
}
