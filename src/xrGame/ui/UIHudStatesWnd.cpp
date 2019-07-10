#include "stdafx.h"
#include "UIHudStatesWnd.h"
#include "Actor.h"
#include "ActorCondition.h"
#include "EntityCondition.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"
#include "Inventory.h"
#include "RadioactiveZone.h"
#include "UIStatic.h"
#include "UIProgressBar.h"
#include "UIProgressShape.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "ui_arrow.h"
#include "UIInventoryUtilities.h"
#include "CustomDetector.h"
#include "ai/Monsters/BaseMonster/base_monster.h"
#include "PDA.h"
#include "WeaponMagazinedWGrenade.h"

CUIHudStatesWnd::CUIHudStatesWnd()
    : m_b_force_update(true), m_timer_1sec(0), m_last_health(0.0f), m_radia_self(0.0f), m_radia_hit(0.0f)
{
    for (int i = 0; i < ALife::infl_max_count; ++i)
    {
        m_zone_cur_power[i] = 0.0f;
        //--		m_zone_max_power[i] = 1.0f;
        m_zone_feel_radius[i] = 1.0f;
    }
    m_zone_hit_type[ALife::infl_rad] = ALife::eHitTypeRadiation;
    m_zone_hit_type[ALife::infl_fire] = ALife::eHitTypeBurn;
    m_zone_hit_type[ALife::infl_acid] = ALife::eHitTypeChemicalBurn;
    m_zone_hit_type[ALife::infl_psi] = ALife::eHitTypeTelepatic;
    m_zone_hit_type[ALife::infl_electra] = ALife::eHitTypeShock;

    m_zone_feel_radius_max = 0.0f;

    m_health_blink = pSettings->r_float("actor_condition", "hud_health_blink");
    clamp(m_health_blink, 0.0f, 1.0f);

    m_fake_indicators_update = false;
    //-	Load_section();
}

CUIHudStatesWnd::~CUIHudStatesWnd() {}
void CUIHudStatesWnd::reset_ui()
{
    if (g_pGameLevel)
    {
        Level().hud_zones_list->clear();
    }
}

ALife::EInfluenceType CUIHudStatesWnd::get_indik_type(ALife::EHitType hit_type)
{
    ALife::EInfluenceType iz_type = ALife::infl_max_count;
    switch (hit_type)
    {
    case ALife::eHitTypeRadiation: iz_type = ALife::infl_rad; break;
    case ALife::eHitTypeLightBurn:
    case ALife::eHitTypeBurn: iz_type = ALife::infl_fire; break;
    case ALife::eHitTypeChemicalBurn: iz_type = ALife::infl_acid; break;
    case ALife::eHitTypeTelepatic: iz_type = ALife::infl_psi; break;
    case ALife::eHitTypeShock: iz_type = ALife::infl_electra; break; // it hasnt CStatic

    case ALife::eHitTypeStrike:
    case ALife::eHitTypeWound:
    case ALife::eHitTypeExplosion:
    case ALife::eHitTypeFireWound:
    case ALife::eHitTypeWound_2:
        //	case ALife::eHitTypePhysicStrike:
        return ALife::infl_max_count;
    default: NODEFAULT;
    }
    return iz_type;
}

extern int __type_hud_lost_alpha;
extern int __type_hud_veter_vremeni;
extern int __type_hud_soc;
extern int __type_hud_coc;
extern int __type_hud_cop;

void CUIHudStatesWnd::InitFromXml(CUIXml& xml, LPCSTR path)
{
    CUIXmlInit::InitWindow(xml, path, 0, this);
    XML_NODE stored_root = xml.GetLocalRoot();

    XML_NODE new_root = xml.NavigateToNode(path, 0);
    xml.SetLocalRoot(new_root);

    m_back = UIHelper::CreateStatic(xml, "back", this);
    m_ui_health_bar = UIHelper::CreateProgressBar(xml, "progress_bar_health", this);
    m_ui_stamina_bar = UIHelper::CreateProgressBar(xml, "progress_bar_stamina", this);
// Убрал m_..._lvl_0,1,2,3->Show(true\false);
// Прописывать их сюда не имеет значения т.к false перекрывает true в функции UpdateIndicators, приводит к багам, когда
// красный значок горит за желтым и т.д -> путаница
#ifdef LOST_ALPHA_HUD_IND
	if (__type_hud_lost_alpha)
	{
    m_bleeding_lvl_0 = UIHelper::CreateStatic(xml, "bleeding_lvl_0", this);
    m_bleeding_lvl_1 = UIHelper::CreateStatic(xml, "bleeding_lvl_1", this);
    m_bleeding_lvl_2 = UIHelper::CreateStatic(xml, "bleeding_lvl_2", this);
    m_bleeding_lvl_3 = UIHelper::CreateStatic(xml, "bleeding_lvl_3", this);

    m_starvation_lvl_0 = UIHelper::CreateStatic(xml, "satiety_lvl_0", this);
    m_starvation_lvl_1 = UIHelper::CreateStatic(xml, "satiety_lvl_1", this);
    m_starvation_lvl_2 = UIHelper::CreateStatic(xml, "satiety_lvl_2", this);
    m_starvation_lvl_3 = UIHelper::CreateStatic(xml, "satiety_lvl_3", this);

    m_overweight_lvl_0 = UIHelper::CreateStatic(xml, "overweight_lvl_0", this);
    m_overweight_lvl_1 = UIHelper::CreateStatic(xml, "overweight_lvl_1", this);
    m_overweight_lvl_2 = UIHelper::CreateStatic(xml, "overweight_lvl_2", this);
    m_overweight_lvl_3 = UIHelper::CreateStatic(xml, "overweight_lvl_3", this);

    m_broken_helmet_lvl_0 = UIHelper::CreateStatic(xml, "broken_helmet_lvl_0", this);
    m_broken_helmet_lvl_1 = UIHelper::CreateStatic(xml, "broken_helmet_lvl_1", this);
    m_broken_helmet_lvl_2 = UIHelper::CreateStatic(xml, "broken_helmet_lvl_2", this);
    m_broken_helmet_lvl_3 = UIHelper::CreateStatic(xml, "broken_helmet_lvl_3", this);

    m_broken_weapon_lvl_0 = UIHelper::CreateStatic(xml, "broken_weapon_lvl_0", this);
    m_broken_weapon_lvl_1 = UIHelper::CreateStatic(xml, "broken_weapon_lvl_1", this);
    m_broken_weapon_lvl_2 = UIHelper::CreateStatic(xml, "broken_weapon_lvl_2", this);
    m_broken_weapon_lvl_3 = UIHelper::CreateStatic(xml, "broken_weapon_lvl_3", this);

    m_broken_armor_lvl_0 = UIHelper::CreateStatic(xml, "broken_armor_lvl_0", this);
    m_broken_armor_lvl_1 = UIHelper::CreateStatic(xml, "broken_armor_lvl_1", this);
    m_broken_armor_lvl_2 = UIHelper::CreateStatic(xml, "broken_armor_lvl_2", this);
    m_broken_armor_lvl_3 = UIHelper::CreateStatic(xml, "broken_armor_lvl_3", this);

    m_thirst_lvl_0 = UIHelper::CreateStatic(xml, "thirst_lvl_0", this);
    m_thirst_lvl_1 = UIHelper::CreateStatic(xml, "thirst_lvl_1", this);
    m_thirst_lvl_2 = UIHelper::CreateStatic(xml, "thirst_lvl_2", this);
    m_thirst_lvl_3 = UIHelper::CreateStatic(xml, "thirst_lvl_3", this);

    m_sleep_lvl_0 = UIHelper::CreateStatic(xml, "sleeping_lvl_0", this);
    m_sleep_lvl_1 = UIHelper::CreateStatic(xml, "sleeping_lvl_1", this);
    m_sleep_lvl_2 = UIHelper::CreateStatic(xml, "sleeping_lvl_2", this);
    m_sleep_lvl_3 = UIHelper::CreateStatic(xml, "sleeping_lvl_3", this);
	}
#endif
#ifdef NEWIND
	if (__type_veter_vremeni || __type_hud_lost_alpha)
	{
    m_ui_psy_health = UIHelper::CreateProgressBar(xml, "progress_bar_PsyHealth", this);
	}
	if (__type_hud_veter_vremeni || __type_hud_soc)
	{
    m_ui_armor_bar = UIHelper::CreateProgressBar(xml, "progress_bar_armor", this);
	m_static_armor = UIHelper::CreateStatic(xml, "static_armor", this);
	m_bleeding = UIHelper::CreateStatic(xml, "bleeding", this);
    m_bleeding->Show(false);
	}
    m_eff_bleeding = UIHelper::CreateStatic(xml, "eff_bleeding_screen", this);
    m_eff_bleeding->Show(false);
	if (__type_hud_soc || __type_hud_cop || __type_hud_veter_vremeni || __type_hud_coc)
	{
    m_psy_grenn = UIHelper::CreateStatic(xml, "GetPsy_grenn", this);
    m_psy_yellow = UIHelper::CreateStatic(xml, "GetPsy_yellow", this);
    m_psy_red = UIHelper::CreateStatic(xml, "GetPsy_red", this);
	}
	if (__type_hud_soc || __type_hud_lost_alpha)
	{
    m_weapon_ammo = UIHelper::CreateTextWnd(xml, "ammo_name", this);
	}
#endif
    m_indik[ALife::infl_rad] = UIHelper::CreateStatic(xml, "indik_rad", this);
    m_indik[ALife::infl_fire] = UIHelper::CreateStatic(xml, "indik_fire", this);
    m_indik[ALife::infl_acid] = UIHelper::CreateStatic(xml, "indik_acid", this);
    m_indik[ALife::infl_psi] = UIHelper::CreateStatic(xml, "indik_psi", this);

    m_ui_weapon_cur_ammo = UIHelper::CreateTextWnd(xml, "static_cur_ammo", this);
    m_ui_weapon_fmj_ammo = UIHelper::CreateTextWnd(xml, "static_fmj_ammo", this);
    m_ui_weapon_ap_ammo = UIHelper::CreateTextWnd(xml, "static_ap_ammo", this);
    m_ui_weapon_third_ammo =
        UIHelper::CreateTextWnd(xml, "static_third_ammo", this); // Alundaio: Option to display a third ammo type

    m_ui_weapon_ammo_color_active = CUIXmlInit::GetColor(xml, "active_ammo_color", 0, color_rgba(238, 155, 23, 255));
    m_ui_weapon_ammo_color_inactive =
        CUIXmlInit::GetColor(xml, "inactive_ammo_color", 0, color_rgba(238, 155, 23, 150));

    m_fire_mode = UIHelper::CreateTextWnd(xml, "static_fire_mode", this);
    m_ui_grenade = UIHelper::CreateTextWnd(xml, "static_grenade", this);

    m_ui_weapon_icon = UIHelper::CreateStatic(xml, "static_wpn_icon", this);
    m_ui_weapon_icon->SetShader(InventoryUtilities::GetEquipmentIconsShader());
    m_ui_weapon_icon_rect = m_ui_weapon_icon->GetWndRect();
    xml.SetLocalRoot(stored_root);
}

void CUIHudStatesWnd::on_connected() { Load_section(); }
void CUIHudStatesWnd::Load_section()
{
    VERIFY(g_pGameLevel);
    if (!Level().hud_zones_list)
    {
        Level().create_hud_zones_list();
        VERIFY(Level().hud_zones_list);
    }
    Level().hud_zones_list->load("all_zone_detector", "zone");
    Load_section_type(ALife::infl_rad, "radiation_zone_detector");
    Load_section_type(ALife::infl_fire, "fire_zone_detector");
    Load_section_type(ALife::infl_acid, "acid_zone_detector");
    Load_section_type(ALife::infl_psi, "psi_zone_detector");
    Load_section_type(ALife::infl_electra, "electra_zone_detector"); // no uistatic
}

void CUIHudStatesWnd::Load_section_type(ALife::EInfluenceType type, LPCSTR section)
{
    m_zone_feel_radius[type] = pSettings->r_float(section, "zone_radius");
    if (m_zone_feel_radius[type] <= 0.0f)
    {
        m_zone_feel_radius[type] = 1.0f;
    }
    if (m_zone_feel_radius_max < m_zone_feel_radius[type])
    {
        m_zone_feel_radius_max = m_zone_feel_radius[type];
    }
    m_zone_threshold[type] = pSettings->r_float(section, "threshold");
}

void CUIHudStatesWnd::Update()
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor)
    {
        return;
    }

    UpdateHealth(actor);
    UpdateActiveItemInfo(actor);
    UpdateIndicators(actor);

    UpdateZones();

    inherited::Update();
}

void CUIHudStatesWnd::UpdateHealth(CActor* actor)
{
    float cur_health = actor->GetfHealth();
    m_ui_health_bar->SetProgressPos(iCeil(cur_health * 100.0f * 35.f) / 35.f);
    if (_abs(cur_health - m_last_health) > m_health_blink)
    {
        m_last_health = cur_health;
        m_ui_health_bar->m_UIProgressItem.ResetColorAnimation();
    }

    float cur_stamina = actor->conditions().GetPower();
    m_ui_stamina_bar->SetProgressPos(iCeil(cur_stamina * 100.0f * 35.f) / 35.f);
    if (!actor->conditions().IsCantSprint())
    {
        m_ui_stamina_bar->m_UIProgressItem.ResetColorAnimation();
    }

#ifdef NEWIND
	if (__type_veter_vremeni || __type_hud_lost_alpha)
	{
    float cur_psy_health = actor->conditions().GetPsyHealth(); // actor->GetPsyHealth();
    m_ui_psy_health->SetProgressPos(iCeil(cur_psy_health * 100.0f * 35.f) / 35.f);
    if (_abs(cur_psy_health - m_last_psyhealth) > m_psy_health_blink)
    {
        m_last_psyhealth = cur_psy_health;
        m_ui_psy_health->m_UIProgressItem.ResetColorAnimation();
    }
	}
	if (__type_hud_veter_vremeni || __type_hud_soc)
	{
    CCustomOutfit* outfit_ = actor->GetOutfit(); //переопределение _
    if (outfit_)
    {
        m_static_armor->Show(true);
        m_ui_armor_bar->Show(true);
        m_ui_armor_bar->SetProgressPos(outfit_->GetCondition() * 100.0f);
    }
    else
    {
        m_static_armor->Show(false);
        m_ui_armor_bar->Show(false);
    }
	}
	if (__type_veter_vremeni)
	{
    if (actor->conditions().BleedingSpeed() > 0.01f)
    {
        m_bleeding->Show(true);
    }
    else
    {
        m_bleeding->Show(false);
    }
	}
    //Эффект кровотечения на экране из Мизери, чтобы не заменять значок кровотечения, нормально работает только при
    //откл. худе, Онли реализм, но убирать не буду ради опциональности худов
    if (actor->conditions().BleedingSpeed() > 0.01f)
    {
        m_eff_bleeding->Show(true);
    }
    else
    {
        m_eff_bleeding->Show(false);
    }
	if (__type_hud_soc || __type_hud_cop || __type_hud_veter_vremeni || __type_hud_coc)
	{
    float psy_koef = actor->conditions().GetPsyHealth();
    //2 проверки чтобы отключались при else if
    m_psy_grenn->Show(false);
    m_psy_yellow->Show(false);
    m_psy_red->Show(false);
    if (psy_koef > 0.70f)
    {
        m_psy_grenn->Show(false);
        m_psy_yellow->Show(false);
        m_psy_red->Show(false);
    }
    else
    {
        if (psy_koef < 0.70f)
        {
            m_psy_grenn->Show(true);
        }
        else if (psy_koef < 0.50f)
        {
            m_psy_grenn->Show(false);
            m_psy_yellow->Show(true);
        }
        else if (psy_koef < 0.30f)
        {
            m_psy_red->Show(true);
            m_psy_yellow->Show(false);
        }
    }
	}
#endif
}

void CUIHudStatesWnd::UpdateActiveItemInfo(CActor* actor)
{
    PIItem item = actor->inventory().ActiveItem();
    if (item)
    {
        if (m_b_force_update)
        {
            if (item->cast_weapon())
                item->cast_weapon()->ForceUpdateAmmo();
            m_b_force_update = false;
        }

        item->GetBriefInfo(m_item_info);

        m_fire_mode->SetText(m_item_info.fire_mode.c_str());
        SetAmmoIcon(m_item_info.icon.c_str());
#ifdef NEWIND
	if (__type_hud_soc || __type_hud_lost_alpha)
	{
        m_weapon_ammo->SetText(m_item_info.name.c_str());
	}
#endif
        m_ui_weapon_cur_ammo->Show(true);
        m_ui_weapon_fmj_ammo->Show(true);
        m_ui_weapon_ap_ammo->Show(true);
        m_ui_weapon_third_ammo->Show(true);

        m_fire_mode->Show(true);
        m_ui_grenade->Show(true);
#ifdef NEWIND
	if (__type_hud_soc || __type_hud_lost_alpha)
	{
        m_weapon_ammo->Show(true);
	}
#endif
        m_ui_weapon_cur_ammo->SetText(m_item_info.cur_ammo.c_str());
        m_ui_weapon_fmj_ammo->SetText(m_item_info.fmj_ammo.c_str());
        m_ui_weapon_ap_ammo->SetText(m_item_info.ap_ammo.c_str());

        // Alundaio: Third ammo type and also set text color for each ammo type
        if (m_ui_weapon_third_ammo)
            m_ui_weapon_third_ammo->SetText(m_item_info.third_ammo.c_str());

        m_ui_grenade->SetText(m_item_info.grenade.c_str());

        m_ui_weapon_fmj_ammo->SetTextColor(m_ui_weapon_ammo_color_inactive);
        m_ui_weapon_ap_ammo->SetTextColor(m_ui_weapon_ammo_color_inactive);
        m_ui_weapon_third_ammo->SetTextColor(m_ui_weapon_ammo_color_inactive);

        CWeaponMagazinedWGrenade* wpn = smart_cast<CWeaponMagazinedWGrenade*>(item);
        if (wpn && wpn->m_bGrenadeMode)
            m_ui_grenade->SetTextColor(m_ui_weapon_ammo_color_active);
        else
            m_ui_grenade->SetTextColor(m_ui_weapon_ammo_color_inactive);

        CWeaponMagazined* wpnm = smart_cast<CWeaponMagazined*>(item);
        if (wpnm)
        {
            if (wpnm->m_ammoType.type1 == 0)
                m_ui_weapon_fmj_ammo->SetTextColor(m_ui_weapon_ammo_color_active);
            else if (wpnm->m_ammoType.type1 == 1)
                m_ui_weapon_ap_ammo->SetTextColor(m_ui_weapon_ammo_color_active);
            else if (wpnm->m_ammoType.type1 == 2)
                m_ui_weapon_third_ammo->SetTextColor(m_ui_weapon_ammo_color_active);
        }
        //-Alundaio
    }
    else
    {
        m_ui_weapon_icon->Show(false);

        m_ui_weapon_cur_ammo->Show(false);
        m_ui_weapon_fmj_ammo->Show(false);
        m_ui_weapon_ap_ammo->Show(false);
        m_ui_weapon_third_ammo->Show(false);
        m_fire_mode->Show(false);
        m_ui_grenade->Show(false);
#ifdef NEWIND
	if (__type_hud_soc || __type_hud_lost_alpha)
	{
        m_weapon_ammo->Show(false);
	}
#endif
    }
}

void CUIHudStatesWnd::SetAmmoIcon(const shared_str& sect_name)
{
    if (!sect_name.size())
    {
        m_ui_weapon_icon->Show(false);
        return;
    }
    m_ui_weapon_icon->Show(true);

    Frect texture_rect;
    texture_rect.x1 = pSettings->r_float(sect_name, "inv_grid_x") * INV_GRID_WIDTH;
    texture_rect.y1 = pSettings->r_float(sect_name, "inv_grid_y") * INV_GRID_HEIGHT;
    texture_rect.x2 = pSettings->r_float(sect_name, "inv_grid_width") * INV_GRID_WIDTH;
    texture_rect.y2 = pSettings->r_float(sect_name, "inv_grid_height") * INV_GRID_HEIGHT;
    texture_rect.rb.add(texture_rect.lt);
    m_ui_weapon_icon->GetUIStaticItem().SetTextureRect(texture_rect);
    m_ui_weapon_icon->SetStretchTexture(true);

    float h = texture_rect.height() * 0.8f;
    float w = texture_rect.width() * 0.8f;

    // now perform only width scale for ammo, which (W)size >2
    if (texture_rect.width() > 2.01f * INV_GRID_WIDTH)
        w = INV_GRID_WIDTH * 1.5f;

    m_ui_weapon_icon->SetWidth(w * UI().get_current_kx());
    m_ui_weapon_icon->SetHeight(h);
}
// ------------------------------------------------------------------------------------------------
void CUIHudStatesWnd::UpdateZones()
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor)
    {
        return;
    }
    CPda* const pda = actor->GetPDA();
    if (pda)
    {
        typedef xr_vector<IGameObject*> monsters;
        for (monsters::const_iterator it = pda->feel_touch.begin(); it != pda->feel_touch.end(); ++it)
        {
            CBaseMonster* const monster = smart_cast<CBaseMonster*>(*it);
            if (!monster || !monster->g_Alive())
                continue;

            monster->play_detector_sound();
        }
    }

    m_radia_self = actor->conditions().GetRadiation();

    float zone_max_power = actor->conditions().GetZoneMaxPower(ALife::infl_rad);
    float power = actor->conditions().GetInjuriousMaterialDamage();
    power = power / zone_max_power;
    clamp(power, 0.0f, 1.1f);
    if (m_zone_cur_power[ALife::infl_rad] < power)
    {
        m_zone_cur_power[ALife::infl_rad] = power;
    }
    m_radia_hit = m_zone_cur_power[ALife::infl_rad];

    if (!Level().hud_zones_list)
    {
        return;
    }

    for (int i = 0; i < ALife::infl_max_count; ++i)
    {
        if (Device.fTimeDelta < 1.0f)
        {
            m_zone_cur_power[i] *= 0.9f * (1.0f - Device.fTimeDelta);
        }
        if (m_zone_cur_power[i] < 0.01f)
        {
            m_zone_cur_power[i] = 0.0f;
        }
    }

    Fvector posf;
    posf.set(Level().CurrentControlEntity()->Position());
    Level().hud_zones_list->feel_touch_update(posf, m_zone_feel_radius_max);

    if (Level().hud_zones_list->m_ItemInfos.size() == 0)
    {
        return;
    }

    CZoneList::ItemsMapIt itb = Level().hud_zones_list->m_ItemInfos.begin();
    CZoneList::ItemsMapIt ite = Level().hud_zones_list->m_ItemInfos.end();
    for (; itb != ite; ++itb)
    {
        CCustomZone* pZone = itb->first;
        ITEM_INFO& zone_info = itb->second;
        ITEM_TYPE* zone_type = zone_info.curr_ref;

        ALife::EHitType hit_type = pZone->GetHitType();
        ALife::EInfluenceType z_type = get_indik_type(hit_type);
        /*		if ( z_type == indik_type_max )
                {
                    continue;
                }
        */

        Fvector P = Level().CurrentControlEntity()->Position();
        P.y -= 0.5f;
        float dist_to_zone = 0.0f;
        float rad_zone = 0.0f;
        pZone->CalcDistanceTo(P, dist_to_zone, rad_zone);
        clamp(dist_to_zone, 0.0f, flt_max * 0.5f);

        float fRelPow =
            (dist_to_zone / (rad_zone + (z_type == ALife::infl_max_count) ? 5.0f : m_zone_feel_radius[z_type] + 0.1f)) -
            0.1f;

        zone_max_power = actor->conditions().GetZoneMaxPower(z_type);
        power = pZone->Power(dist_to_zone, rad_zone);
        // power = power / zone_max_power;
        clamp(power, 0.0f, 1.1f);

        if ((z_type != ALife::infl_max_count) && (m_zone_cur_power[z_type] < power)) // max
        {
            m_zone_cur_power[z_type] = power;
        }

        if (dist_to_zone < rad_zone + 0.9f * ((z_type == ALife::infl_max_count) ? 5.0f : m_zone_feel_radius[z_type]))
        {
            fRelPow *= 0.6f;
            if (dist_to_zone < rad_zone)
            {
                fRelPow *= 0.3f;
                fRelPow *= (2.5f - 2.0f * power); // звук зависит от силы зоны
            }
        }
        clamp(fRelPow, 0.0f, 1.0f);

        //определить текущую частоту срабатывания сигнала
        zone_info.cur_period = zone_type->freq.x + (zone_type->freq.y - zone_type->freq.x) * (fRelPow * fRelPow);

        if (zone_info.snd_time > zone_info.cur_period)
        {
            zone_info.snd_time = 0.0f;
            HUD_SOUND_ITEM::PlaySound(zone_type->detect_snds, Fvector().set(0, 0, 0), NULL, true, false);
        }
        else
        {
            zone_info.snd_time += Device.fTimeDelta;
        }
    } // for itb
}

void CUIHudStatesWnd::UpdateIndicators(CActor* actor)
{
    if (m_fake_indicators_update)
        return;

    for (int i = 0; i < it_max; ++i) // it_max = ALife::infl_max_count-1
    {
        UpdateIndicatorType(actor, (ALife::EInfluenceType)i);
    }
#ifdef LOST_ALPHA_HUD_IND
	if (__type_hud_lost_alpha)
{
    // Код в UIMainIngameWnd очень старый, не совместим с новыми функциями. Пришлось переносить сюда
    // Потому что эти статики накладываются ПОВЕРХ статика _back
    float satiety = actor->conditions().GetSatiety();
    float satiety_critical = actor->conditions().SatietyCritical();
    float satiety_koef =
        (satiety - satiety_critical) / (satiety >= satiety_critical ? 1 - satiety_critical : satiety_critical);

    m_starvation_lvl_1->Show(false);
    m_starvation_lvl_2->Show(false);
    m_starvation_lvl_3->Show(false);

    if (satiety_koef > 0.5)
    {
        m_starvation_lvl_0->Show(true);
    }
    else
    {
        if (satiety_koef > 0.0f)
        {
            m_starvation_lvl_0->Show(false);
            m_starvation_lvl_1->Show(true);
        }
        else if (satiety_koef > -0.5f)
        {
            m_starvation_lvl_0->Show(false);
            m_starvation_lvl_2->Show(true);
        }
        else if (satiety_koef < 0.0f)
        {
            m_starvation_lvl_0->Show(false);
            m_starvation_lvl_3->Show(true);
        }
    }

    float bleeding_float = actor->conditions().BleedingSpeed();
    m_bleeding_lvl_1->Show(false);
    m_bleeding_lvl_2->Show(false);
    m_bleeding_lvl_3->Show(false);
    if (fis_zero(bleeding_float, EPS))
    {
        m_bleeding_lvl_0->Show(true); // проверка для else if
    }
    else
    {
        if (bleeding_float < 0.35f)
        {
            m_bleeding_lvl_0->Show(false);
            m_bleeding_lvl_1->Show(true);
        }
        else if (bleeding_float < 0.7f)
        {
            m_bleeding_lvl_0->Show(false);
            m_bleeding_lvl_2->Show(true);
        }
        else if (bleeding_float > 0.35f)
        {
            m_bleeding_lvl_0->Show(false);
            m_bleeding_lvl_3->Show(true);
        }
    }

    float cur_weight = actor->inventory().TotalWeight();
    float max_weight = actor->MaxWalkWeight();
    float max_carry_weight = actor->MaxCarryWeight();
    m_overweight_lvl_0->Show(true);
    m_overweight_lvl_1->Show(false);
    m_overweight_lvl_2->Show(false);
    m_overweight_lvl_3->Show(false);
    if (cur_weight >= max_carry_weight)
    {
        if (cur_weight >= max_weight)
        {
            m_overweight_lvl_0->Show(false);
            m_overweight_lvl_3->Show(true);
        }
        else if (max_carry_weight / max_weight >= 0.5f)
        {
            m_overweight_lvl_0->Show(false);
            m_overweight_lvl_2->Show(true);
        }
        else if (max_carry_weight / max_weight <= 0.5f)
        {
            m_overweight_lvl_0->Show(false);
            m_overweight_lvl_1->Show(true);
        }
    }

    u16 slot = actor->inventory().GetActiveSlot();
    m_broken_weapon_lvl_0->Show(true);
    m_broken_weapon_lvl_1->Show(false);
    m_broken_weapon_lvl_2->Show(false);
    m_broken_weapon_lvl_3->Show(false);
    if (slot == INV_SLOT_2 || slot == INV_SLOT_3)
    {
        CWeapon* weapon = smart_cast<CWeapon*>(actor->inventory().ItemFromSlot(slot));
        if (weapon)
        {
            float condition = weapon->GetCondition();
            float start_misf_cond = weapon->GetMisfireStartCondition();
            float end_misf_cond = weapon->GetMisfireEndCondition();
            if (condition < start_misf_cond)
            {
                if (condition > (start_misf_cond + end_misf_cond) / 2)
                { //зеленый
                    m_broken_weapon_lvl_0->Show(false);
                    m_broken_weapon_lvl_1->Show(true);
                }

                else if (condition > end_misf_cond)
                { //желтый
                    m_broken_weapon_lvl_0->Show(false);
                    m_broken_weapon_lvl_2->Show(true);
                }
                else if (condition < (start_misf_cond + end_misf_cond) / 2) //красный
                {
                    m_broken_weapon_lvl_0->Show(false);
                    m_broken_weapon_lvl_3->Show(true);
                }
            }
        }
    }

    CHelmet* helmet = smart_cast<CHelmet*>(actor->inventory().ItemFromSlot(HELMET_SLOT));
    m_broken_helmet_lvl_0->Show(true);
    m_broken_helmet_lvl_1->Show(false);
    m_broken_helmet_lvl_2->Show(false);
    m_broken_helmet_lvl_3->Show(false);
    if (helmet)
    {
        float condition = helmet->GetCondition();
        if (condition < 0.75f)
        {
            if (condition > 0.5f)
            { //зеленый
                m_broken_helmet_lvl_0->Show(false);
                m_broken_helmet_lvl_1->Show(true);
            }
            else if (condition > 0.25f)
            { //желтый
                m_broken_helmet_lvl_0->Show(false);
                m_broken_helmet_lvl_2->Show(true);
            }
            else if (condition < 0.5f) // красный
            {
                m_broken_helmet_lvl_0->Show(false);
                m_broken_helmet_lvl_3->Show(true);
            }
        }
    }

    CCustomOutfit* outfit = smart_cast<CCustomOutfit*>(actor->inventory().ItemFromSlot(OUTFIT_SLOT));
    m_broken_armor_lvl_0->Show(true);
    m_broken_armor_lvl_1->Show(false);
    m_broken_armor_lvl_2->Show(false);
    m_broken_armor_lvl_3->Show(false);
    if (outfit)
    {
        float condition = outfit->GetCondition();
        if (condition < 0.75f)
        {
            if (condition > 0.5f) // зеленый
            {
                m_broken_armor_lvl_0->Show(false);
                m_broken_armor_lvl_1->Show(true);
            }
            else if (condition > 0.25f)
            { // желтый
                m_broken_armor_lvl_0->Show(false);
                m_broken_armor_lvl_2->Show(true);
            }
            else if (condition < 0.5f)
            {
                m_broken_armor_lvl_0->Show(false);
                m_broken_armor_lvl_3->Show(true);
            }
        }
    }

    float thirst = actor->conditions().GetThirst();
    float thirst_critical = actor->conditions().ThirstCritical();
    float thirst_koef =
        (thirst - thirst_critical) / (thirst >= thirst_critical ? 1 - thirst_critical : thirst_critical);
    m_thirst_lvl_0->Show(true);
    m_thirst_lvl_1->Show(false);
    m_thirst_lvl_2->Show(false);
    m_thirst_lvl_3->Show(false);
    if (thirst_koef > 0.5)
    {
        m_thirst_lvl_0->Show(true);
    }
    else
    {
        if (thirst_koef > 0.0f)
        {
            m_thirst_lvl_0->Show(false);
            m_thirst_lvl_1->Show(true);
        }
        else if (thirst_koef > -0.5f)
        {
            m_thirst_lvl_0->Show(false);
            m_thirst_lvl_2->Show(true);
        }
        else if (thirst_koef < 0.0f)
        {
            m_thirst_lvl_0->Show(false);
            m_thirst_lvl_3->Show(true);
        }
    }

    float sleep = actor->conditions().GetSleep();
    float sleep_critical = actor->conditions().SleepCritical();
    float sleep_koef = (sleep - sleep_critical) / (sleep >= sleep_critical ? 1 - sleep_critical : sleep_critical);
    m_sleep_lvl_0->Show(true);
    m_sleep_lvl_1->Show(false);
    m_sleep_lvl_2->Show(false);
    m_sleep_lvl_3->Show(false);
    if (sleep_koef > 0.5)
    {
        m_sleep_lvl_0->Show(true);
    }
    else
    {
        if (sleep_koef > 0.0f)
        {
            m_sleep_lvl_0->Show(false);
            m_sleep_lvl_1->Show(true);
        }
        else if (sleep_koef > -0.5f)
        {
            m_sleep_lvl_0->Show(false);
            m_sleep_lvl_2->Show(true);
        }
        else if (sleep_koef < 0.0f)
        {
            m_sleep_lvl_0->Show(false);
            m_sleep_lvl_3->Show(true);
        }
    }
}
#endif
}

void CUIHudStatesWnd::UpdateIndicatorType(CActor* actor, ALife::EInfluenceType type)
{
    if (type < ALife::infl_rad || ALife::infl_psi < type)
    {
        VERIFY2(0, "Failed EIndicatorType for CStatic!");
        return;
    }

    /*
        u32 c_white  = color_rgba( 255, 255, 255, 255 );
        u32 c_green  = color_rgba( 0, 255, 0, 255 );
        u32 c_yellow = color_rgba( 255, 255, 0, 255 );
        u32 c_red    = color_rgba( 255, 0, 0, 255 );
    */
    LPCSTR texture = "";
    string128 str;
    switch (type)
    {
    case ALife::infl_rad: texture = "ui_inGame2_triangle_Radiation_"; break;
    case ALife::infl_fire: texture = "ui_inGame2_triangle_Fire_"; break;
    case ALife::infl_acid: texture = "ui_inGame2_triangle_Biological_"; break;
    case ALife::infl_psi: texture = "ui_inGame2_triangle_Psy_"; break;
    default: NODEFAULT;
    }
    float hit_power = m_zone_cur_power[type];
    ALife::EHitType hit_type = m_zone_hit_type[type];

    CCustomOutfit* outfit = actor->GetOutfit();
    CHelmet* helmet = smart_cast<CHelmet*>(actor->inventory().ItemFromSlot(HELMET_SLOT));
    float protect = (outfit) ? outfit->GetDefHitTypeProtection(hit_type) : 0.0f;
    protect += (helmet) ? helmet->GetDefHitTypeProtection(hit_type) : 0.0f;
    protect += actor->GetProtection_ArtefactsOnBelt(hit_type);

    CEntityCondition::BOOSTER_MAP& cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
    CEntityCondition::BOOSTER_MAP::const_iterator it;
    if (hit_type == ALife::eHitTypeChemicalBurn)
    {
        it = cur_booster_influences.find(eBoostChemicalBurnProtection);
        if (it != cur_booster_influences.end())
            protect += it->second.fBoostValue;
    }
    else if (hit_type == ALife::eHitTypeRadiation)
    {
        it = cur_booster_influences.find(eBoostRadiationProtection);
        if (it != cur_booster_influences.end())
            protect += it->second.fBoostValue;
    }
    else if (hit_type == ALife::eHitTypeTelepatic)
    {
        it = cur_booster_influences.find(eBoostTelepaticProtection);
        if (it != cur_booster_influences.end())
            protect += it->second.fBoostValue;
    }

    //	float max_power = actor->conditions().GetZoneMaxPower( hit_type );
    //	protect = protect / max_power; // = 0..1

    if (hit_power < EPS)
    {
        m_indik[type]->Show(false);
        //		m_indik[type]->SetTextureColor( c_white );
        //		SwitchLA( false, type );
        actor->conditions().SetZoneDanger(0.0f, type);
        return;
    }

    m_indik[type]->Show(true);
    if (hit_power <= protect)
    {
        //		m_indik[type]->SetTextureColor( c_green );
        //		SwitchLA( false, type );
        xr_sprintf(str, sizeof(str), "%s%s", texture, "green");
        texture = str;
        m_indik[type]->InitTexture(texture);
        actor->conditions().SetZoneDanger(0.0f, type);
        return;
    }
    if (hit_power - protect < m_zone_threshold[type])
    {
        //		m_indik[type]->SetTextureColor( c_yellow );
        //		SwitchLA( false, type );
        xr_sprintf(str, sizeof(str), "%s%s", texture, "yellow");
        texture = str;
        m_indik[type]->InitTexture(texture);
        actor->conditions().SetZoneDanger(0.0f, type);
        return;
    }
    //	m_indik[type]->SetTextureColor( c_red );
    //	SwitchLA( true, type );
    xr_sprintf(str, sizeof(str), "%s%s", texture, "red");
    texture = str;
    m_indik[type]->InitTexture(texture);
    VERIFY(actor->conditions().GetZoneMaxPower(hit_type));
    actor->conditions().SetZoneDanger((hit_power - protect) / actor->conditions().GetZoneMaxPower(hit_type), type);
}
/*
void CUIHudStatesWnd::SwitchLA( bool state, ALife::EInfluenceType type )
{
    if ( state == m_cur_state_LA[type] )
    {
        return;
    }

    if ( state )
    {
        m_indik[type]->SetColorAnimation( m_lanim_name.c_str(), LA_CYCLIC|LA_TEXTURECOLOR);
        m_cur_state_LA[type] = true;
    }
    else
    {
        m_indik[type]->SetColorAnimation( NULL, 0);//off
        m_cur_state_LA[type] = false;
    }
}
*/
float CUIHudStatesWnd::get_zone_cur_power(ALife::EHitType hit_type)
{
    ALife::EInfluenceType iz_type = get_indik_type(hit_type);
    if (iz_type == ALife::infl_max_count)
    {
        return 0.0f;
    }
    return m_zone_cur_power[iz_type];
}

void CUIHudStatesWnd::DrawZoneIndicators()
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor)
        return;

    UpdateIndicators(actor);

    if (m_indik[ALife::infl_rad]->IsShown())
        m_indik[ALife::infl_rad]->Draw();

    if (m_indik[ALife::infl_fire]->IsShown())
        m_indik[ALife::infl_fire]->Draw();

    if (m_indik[ALife::infl_acid]->IsShown())
        m_indik[ALife::infl_acid]->Draw();

    if (m_indik[ALife::infl_psi]->IsShown())
        m_indik[ALife::infl_psi]->Draw();
}

void CUIHudStatesWnd::FakeUpdateIndicatorType(u8 t, float power)
{
    ALife::EInfluenceType type = (ALife::EInfluenceType)t;
    if (type < ALife::infl_rad || ALife::infl_psi < type)
    {
        VERIFY2(0, "Failed EIndicatorType for CStatic!");
        return;
    }

    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor)
        return;

    LPCSTR texture = "";
    string128 str;
    switch (type)
    {
    case ALife::infl_rad: texture = "ui_inGame2_triangle_Radiation_"; break;
    case ALife::infl_fire: texture = "ui_inGame2_triangle_Fire_"; break;
    case ALife::infl_acid: texture = "ui_inGame2_triangle_Biological_"; break;
    case ALife::infl_psi: texture = "ui_inGame2_triangle_Psy_"; break;
    default: NODEFAULT;
    }
    float hit_power = power;
    ALife::EHitType hit_type = m_zone_hit_type[type];

    CCustomOutfit* outfit = actor->GetOutfit();
    CHelmet* helmet = smart_cast<CHelmet*>(actor->inventory().ItemFromSlot(HELMET_SLOT));
    float protect = (outfit) ? outfit->GetDefHitTypeProtection(hit_type) : 0.0f;
    protect += (helmet) ? helmet->GetDefHitTypeProtection(hit_type) : 0.0f;
    protect += actor->GetProtection_ArtefactsOnBelt(hit_type);

    CEntityCondition::BOOSTER_MAP& cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
    CEntityCondition::BOOSTER_MAP::const_iterator it;
    if (hit_type == ALife::eHitTypeChemicalBurn)
    {
        it = cur_booster_influences.find(eBoostChemicalBurnProtection);
        if (it != cur_booster_influences.end())
            protect += it->second.fBoostValue;
    }
    else if (hit_type == ALife::eHitTypeRadiation)
    {
        it = cur_booster_influences.find(eBoostRadiationProtection);
        if (it != cur_booster_influences.end())
            protect += it->second.fBoostValue;
    }
    else if (hit_type == ALife::eHitTypeTelepatic)
    {
        it = cur_booster_influences.find(eBoostTelepaticProtection);
        if (it != cur_booster_influences.end())
            protect += it->second.fBoostValue;
    }

    float max_power = actor->conditions().GetZoneMaxPower(hit_type);
    protect = protect / max_power; // = 0..1

    if (hit_power < EPS)
    {
        m_indik[type]->Show(false);
        actor->conditions().SetZoneDanger(0.0f, type);
        return;
    }

    m_indik[type]->Show(true);
    if (hit_power < protect)
    {
        xr_sprintf(str, sizeof(str), "%s%s", texture, "green");
        texture = str;
        m_indik[type]->InitTexture(texture);
        actor->conditions().SetZoneDanger(0.0f, type);
        return;
    }
    if (hit_power - protect < m_zone_threshold[type])
    {
        xr_sprintf(str, sizeof(str), "%s%s", texture, "yellow");
        texture = str;
        m_indik[type]->InitTexture(texture);
        actor->conditions().SetZoneDanger(0.0f, type);
        return;
    }
    xr_sprintf(str, sizeof(str), "%s%s", texture, "red");
    texture = str;
    m_indik[type]->InitTexture(texture);
    actor->conditions().SetZoneDanger(hit_power - protect, type);
}

void CUIHudStatesWnd::EnableFakeIndicators(bool enable) { m_fake_indicators_update = enable; }
