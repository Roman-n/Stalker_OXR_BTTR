#pragma once
#ifdef CHIMERA_CS
#include "Chimera_cs_state_threaten_steal.h"
#include "Chimera_cs_state_threaten_walk.h"
#include "Chimera_cs_state_threaten_roar.h"


#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimera_csThreatenAbstract CStateChimera_csThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimera_csThreatenAbstract::CStateChimera_csThreaten(_Object *obj) : inherited(obj)
{
	add_state(eStateWalk,		new CStateChimera_csThreatenWalk<_Object> 	(obj));
	add_state(eStateThreaten,	new CStateChimera_csThreatenRoar<_Object> 	(obj));
	add_state(eStateSteal,		new CStateChimera_csThreatenSteal<_Object> (obj));
}

TEMPLATE_SPECIALIZATION
CStateChimera_csThreatenAbstract::~CStateChimera_csThreaten()
{
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenAbstract::reinit()
{
	inherited::reinit	();

	m_last_time_threaten = 0;
}


#define MIN_DIST_TO_ENEMY	3.f
#define MORALE_THRESHOLD	0.8f
#define THREATEN_DELAY		10000

TEMPLATE_SPECIALIZATION
bool CStateChimera_csThreatenAbstract::check_start_conditions()
{
	if (object->tfGetRelationType(object->EnemyMan.get_enemy()) == ALife::eRelationTypeWorstEnemy) return false;
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) < MIN_DIST_TO_ENEMY) return false;
	if (object->HitMemory.is_hit())						return false;
	if (object->hear_dangerous_sound)					return false;
	if (m_last_time_threaten + THREATEN_DELAY > Device.dwTimeGlobal) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateChimera_csThreatenAbstract::check_completion()
{
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) < MIN_DIST_TO_ENEMY) return true;
	if (object->HitMemory.is_hit()) return true;
	if (object->tfGetRelationType(object->EnemyMan.get_enemy()) == ALife::eRelationTypeWorstEnemy) return true;

	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenAbstract::initialize()
{
	inherited::initialize	();
	object->SetUpperState	();
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenAbstract::reselect_state()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateThreaten);
		return;
	}

	if (prev_substate == eStateSteal) {
		select_state(eStateThreaten);
		return;
	}

	if (prev_substate == eStateThreaten) {
		if (get_state(eStateSteal)->check_start_conditions()) {
			select_state(eStateSteal);
			return;
		} else if (get_state(eStateWalk)->check_start_conditions()) {
			select_state(eStateWalk);
			return;
		}
	}

	select_state(eStateThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenAbstract::finalize()
{
	inherited::finalize		();
	object->SetUpperState	(false);
	m_last_time_threaten	 = Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->SetUpperState	(false);
	m_last_time_threaten	 = Device.dwTimeGlobal;
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimera_csThreatenAbstract
#endif
