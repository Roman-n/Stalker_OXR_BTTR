#pragma once
#include "xruicore/windows/uiwindow.h"
#include "xrServerEntities/alife_space.h"
#include "xrServerEntities/inventory_space.h"
#include "xrGame/actor_defs.h"

class CUIStatic;
class CUITextWnd;
class CUIProgressBar;
class CUIProgressShape;
class CUIXml;
class UI_Arrow;
class CActor;

int const it_max = ALife::infl_max_count - 1;

class CUIHudStatesWnd : public CUIWindow
{
private:
    typedef CUIWindow inherited;
    //-	typedef ALife::EInfluenceType	EIndicatorType;
public:
    CUIStatic* m_back;
    CUIStatic* m_indik[it_max];

    u32 m_ui_weapon_ammo_color_active;
    u32 m_ui_weapon_ammo_color_inactive;
    CUITextWnd* m_ui_weapon_cur_ammo;
    CUITextWnd* m_ui_weapon_fmj_ammo;
    CUITextWnd* m_ui_weapon_ap_ammo;
    CUITextWnd* m_ui_weapon_third_ammo; //Alundaio
    CUITextWnd* m_fire_mode;
    CUITextWnd* m_ui_grenade;
    CUITextWnd* m_weapon_ammo;
    II_BriefInfo m_item_info;

	CUIProgressBar*	m_ui_psy_health;

    CUIStatic* m_psy_grenn;
    CUIStatic* m_psy_yellow;
    CUIStatic* m_psy_red;
	CUIStatic* m_ui_weapon_sign_ammo;

    CUIStatic* m_bleeding_soc_lvl_1;
    CUIStatic* m_bleeding_soc_lvl_2;
    CUIStatic* m_bleeding_soc_lvl_3;

    CUIStatic* m_starvation_soc_lvl_1;
    CUIStatic* m_starvation_soc_lvl_2;
    CUIStatic* m_starvation_soc_lvl_3;

    CUIStatic* m_overweight_soc_lvl_1;
    CUIStatic* m_overweight_soc_lvl_2;
    CUIStatic* m_overweight_soc_lvl_3;

    CUIStatic* m_broken_helmet_soc_lvl_1;
    CUIStatic* m_broken_helmet_soc_lvl_2;
    CUIStatic* m_broken_helmet_soc_lvl_3;

    CUIStatic* m_broken_weapon_soc_lvl_1;
    CUIStatic* m_broken_weapon_soc_lvl_2;
    CUIStatic* m_broken_weapon_soc_lvl_3;

    CUIStatic* m_thirst_soc_lvl_1;
    CUIStatic* m_thirst_soc_lvl_2;
    CUIStatic* m_thirst_soc_lvl_3;

    CUIStatic* m_sleep_soc_lvl_1;
    CUIStatic* m_sleep_soc_lvl_2;
    CUIStatic* m_sleep_soc_lvl_3;

    CUIStatic* m_radiation_soc_lvl_1;
    CUIStatic* m_radiation_soc_lvl_2;
    CUIStatic* m_radiation_soc_lvl_3;
	
#ifdef LOST_ALPHA_HUD_IND
	CUIStatic* m_bleeding_lvl_0;
	CUIStatic* m_bleeding_lvl_1;
	CUIStatic* m_bleeding_lvl_2;
	CUIStatic* m_bleeding_lvl_3;

    CUIStatic* m_starvation_lvl_0;
    CUIStatic* m_starvation_lvl_1;
    CUIStatic* m_starvation_lvl_2;
    CUIStatic* m_starvation_lvl_3;

    CUIStatic* m_overweight_lvl_0;
    CUIStatic* m_overweight_lvl_1;
    CUIStatic* m_overweight_lvl_2;
    CUIStatic* m_overweight_lvl_3;

    CUIStatic* m_broken_helmet_lvl_0;
    CUIStatic* m_broken_helmet_lvl_1;
    CUIStatic* m_broken_helmet_lvl_2;
    CUIStatic* m_broken_helmet_lvl_3;

    CUIStatic* m_broken_weapon_lvl_0;
    CUIStatic* m_broken_weapon_lvl_1;
    CUIStatic* m_broken_weapon_lvl_2;
    CUIStatic* m_broken_weapon_lvl_3;

    CUIStatic* m_broken_armor_lvl_0;
    CUIStatic* m_broken_armor_lvl_1;
    CUIStatic* m_broken_armor_lvl_2;
    CUIStatic* m_broken_armor_lvl_3;

    CUIStatic* m_thirst_lvl_0;
    CUIStatic* m_thirst_lvl_1;
    CUIStatic* m_thirst_lvl_2;
    CUIStatic* m_thirst_lvl_3;

    CUIStatic* m_sleep_lvl_0;
    CUIStatic* m_sleep_lvl_1;
    CUIStatic* m_sleep_lvl_2;
    CUIStatic* m_sleep_lvl_3;
#endif

	CUIStatic* m_static_health;	
    CUIStatic* m_static_armor;
    CUIStatic* m_bleeding;
    CUIProgressBar* m_ui_armor_bar;	

    CUIStatic* m_ui_weapon_icon;
    Frect m_ui_weapon_icon_rect;

    CUIProgressBar* m_ui_health_bar;
    CUIProgressBar* m_ui_stamina_bar;

    CUIStatic* m_radia_damage;
private:
    float m_last_health;
    float m_health_blink;
    float m_psy_health_blink;
    float m_last_psyhealth;
    float m_radia_self;

    float m_radia_hit;
    shared_str m_lanim_name;

    float m_zone_cur_power[ALife::infl_max_count];
    float m_zone_feel_radius[ALife::infl_max_count];
    ALife::EHitType m_zone_hit_type[ALife::infl_max_count];
    float m_zone_threshold[ALife::infl_max_count];

    float m_zone_feel_radius_max;
    u32 m_timer_1sec;

    bool m_fake_indicators_update;
    //	bool				m_cur_state_LA[it_max];
    bool m_b_force_update;

public:
    CUIHudStatesWnd();
    virtual ~CUIHudStatesWnd();

    void InitFromXml(CUIXml& xml, LPCSTR path);
    void Load_section();
    virtual void Update();
    //	virtual void	Draw				();

    void on_connected();
    void reset_ui();
    void UpdateHealth(CActor* actor);
    void SetAmmoIcon(const shared_str& sect_name);
    void UpdateActiveItemInfo(CActor* actor);

    void UpdateZones();
    void UpdateIndicators(CActor* actor);

    float get_zone_cur_power(ALife::EHitType hit_type);
    float get_main_sensor_value() { return m_radia_hit; }
    void DrawZoneIndicators();
    void FakeUpdateIndicatorType(u8 t, float power);
    void EnableFakeIndicators(bool enable);

protected:
    static ALife::EInfluenceType get_indik_type(ALife::EHitType hit_type);

    void Load_section_type(ALife::EInfluenceType type, LPCSTR section);
    void UpdateIndicatorType(CActor* actor, ALife::EInfluenceType type);
    //			void	SwitchLA			( bool state, ALife::EInfluenceType type );

}; // class CUIHudStatesWnd
