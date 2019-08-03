#include "stdafx.h"
#ifdef POLTERGEIST_CS
#include "poltergeist_cs.h"
#include "poltergeist_cs_state_manager.h"

#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"

#include "poltergeist_cs_state_rest.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "poltergeist_cs_state_attack_hidden.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../../../entitycondition.h"

CStateManagerPoltergeist_cs::CStateManagerPoltergeist_cs(CPoltergeist_cs *obj) : inherited(obj)
{
	add_state(eStateRest,					new CPoltergeist_csStateRest<CPoltergeist_cs>  (obj));
	add_state(eStateEat,					new CStateMonsterEat<CPoltergeist_cs> (obj));
	add_state(eStateAttack,					new CStateMonsterAttack<CPoltergeist_cs> (obj));
	add_state(eStateAttack_AttackHidden,	new CStatePoltergeistAttackHidden<CPoltergeist_cs>  (obj));
	add_state(eStatePanic,					new CStateMonsterPanic<CPoltergeist_cs> (obj));
	add_state(eStateHitted,					new CStateMonsterHitted<CPoltergeist_cs> (obj));
	add_state(eStateHearInterestingSound,	new CStateMonsterHearInterestingSound<CPoltergeist_cs> (obj));
	add_state(eStateHearDangerousSound,		new CStateMonsterHearDangerousSound<CPoltergeist_cs> (obj));
}

CStateManagerPoltergeist_cs::~CStateManagerPoltergeist_cs() {}

void CStateManagerPoltergeist_cs::reinit()
{
	inherited::reinit();
	
	time_next_flame_attack	= 0;
	time_next_tele_attack	= 0;
	time_next_scare_attack	= 0;

}

void CStateManagerPoltergeist_cs::execute()
{
	u32 state_id = u32(-1);
#ifdef NEW_AI_POLTER
    const CEntityAlive* enemy = object->EnemyMan.get_enemy();

    if (enemy)
    {
        switch (object->EnemyMan.get_danger_type()) // Чтобы полтер улетал от НПС, приводит к багу
        {
        case eStrong: 
					state_id = eStatePanic; 
					break; 
        case eWeak: 
					state_id = eStatePanic;
					break;  
        }
    }
    else if (object->HitMemory.is_hit())
    {
        state_id = eStateHitted;
    }
    else if (object->hear_interesting_sound || object->hear_dangerous_sound)
    {
        state_id = eStateHearDangerousSound;
    }
    else
    {
        if (can_eat())
            state_id = eStateEat;
        else
        {
            state_id = eStateRest;	// Если другая анимация, не едим ничего
			object->EnableHide();	// Обратно взлетаем
        }
    }

    if (state_id == eStateEat) {	// Хотим покушать, спустились на землю
    	if (object->CorpseMan.get_corpse()->Position().distance_to(object->Position()) < 10.f) {
    		if (object->is_hidden()) {
    			object->CEnergyHolder::deactivate();
    		}
    		object->DisableHide();
    	}
    }
#else
    state_id = eStateRest;
#endif
	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

#define TIME_SEEN_FOR_FIRE 5000

void CStateManagerPoltergeist_cs::polter_attack(){}
#endif
