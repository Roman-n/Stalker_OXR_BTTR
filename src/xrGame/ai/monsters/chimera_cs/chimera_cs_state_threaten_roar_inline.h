#pragma once
#ifdef CHIMERA_CS
#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateChimera_csThreatenRoarAbstract CStateChimera_csThreatenRoar<_Object>

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenRoarAbstract::initialize()
{
	inherited::initialize	();

}

TEMPLATE_SPECIALIZATION
void CStateChimera_csThreatenRoarAbstract::execute()
{

	object->set_action				(ACT_STAND_IDLE);
	object->anim().SetSpecParams	(ASP_THREATEN);
	object->set_state_sound			(MonsterSound::eMonsterSoundThreaten);
	object->dir().face_target		(object->EnemyMan.get_enemy(), 1200);
}

#define STATE_TIME_OUT	4000

TEMPLATE_SPECIALIZATION
bool CStateChimera_csThreatenRoarAbstract::check_completion()
{	
	if (time_state_started + STATE_TIME_OUT < Device.dwTimeGlobal) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateChimera_csThreatenRoarAbstract

#endif