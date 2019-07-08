#include "stdafx.h"
#ifdef CHIMERA_CS
#include "Chimera_cs.h"
#include "Chimera_cs_state_manager.h"

#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"

#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../states/monster_state_attack_melee.h"
#include "../states/monster_state_attack_run.h"


#include "Chimera_cs_state_threaten.h"
#include "../states/state_test_state.h"
//#include "../group_states/group_state_home_point_attack.h"
#include "Chimera_cs_state_attack_run.h"

CStateManagerChimera_cs::CStateManagerChimera_cs(CChimera_cs *obj) : inherited(obj)
{
//	CStateMonsterAttackMoveToHomePoint<CChimera_cs>* move2home = 
// 	xr_new<CStateMonsterAttackMoveToHomePoint<CChimera_cs> >(obj, true);
	
	add_state(eStateRest,					new CStateMonsterRest<CChimera_cs> 					(obj));
	add_state(eStatePanic,					new CStateMonsterPanic<CChimera_cs> 					(obj));
	
    add_state(eStateAttack,					new CStateChimera_csAttackRun<CChimera_cs> 			    (obj));

// 	add_state(eStateAttack,					xr_new<CStateMonsterAttack<CChimera_cs> >				(obj, move2home));
	add_state(eStateEat,					new CStateMonsterEat<CChimera_cs> 						(obj));
	add_state(eStateHearInterestingSound,	new CStateMonsterHearInterestingSound<CChimera_cs> 	(obj));
	add_state(eStateHearDangerousSound,		new CStateMonsterHearDangerousSound<CChimera_cs> 		(obj));
	add_state(eStateHitted,					 new CStateMonsterHitted<CChimera_cs> 					(obj));
	add_state(eStateThreaten,				new CStateChimera_csThreaten<CChimera_cs> 				(obj));
	add_state(eStateCustom,					new CStateMonsterTestState<CChimera_cs> 				(obj));
}

//	add_state(eStateAttack,					xr_new<CStateMonsterAttack<CChimera_cs> >					(obj));
//  add_state(eStateAttack,					xr_new<CStateChimera_csAttackRun<CChimera_cs> >			    (obj));

CStateManagerChimera_cs::~CStateManagerChimera_cs()
{
}

void CStateManagerChimera_cs::execute()
{
	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy	();

	if (enemy) {
		//if (check_state(eStateThreaten)) state_id = eStateThreaten;
		switch (object->EnemyMan.get_danger_type()) {
			case eStrong:	state_id = eStatePanic; break;
			case eWeak:		state_id = eStateAttack; break;
		}
	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateHearDangerousSound;
	} else if (object->hear_interesting_sound) {
		state_id = eStateHearInterestingSound;
	} else {
		if (can_eat())	state_id = eStateEat;
		else			state_id = eStateRest;
	}
	
	//state_id = eStateCustom;

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}
#endif
